//
// Created by FUQAQ on 2025/3/7.
//

#include "ProcessBusiness.h"

#include "data-process/service/user/UserService.h"
#include "data-process/service/friendship/FriendshipService.h"
#include "data-process/service/group/GroupService.h"
#include "data-process/service/district/DistrictService.h"
#include "data-process/service/file/FileService.h"
#include "data-process/service/message/MessageService.h"
#include "data-process/service/sticker/StickerService.h"

#include "do-business/do-email-code/DoEmailCode.h"
#include "do-business/gen-trtc-sig/GenerateUserSig.h"

#include <unistd.h>
#include <yaml-cpp/yaml.h>

using namespace std;

// 限制子线程操作数据库的并发操作
std::mutex m;
// 限制子线程添加群聊消息的并发操作
std::mutex groupM;

extern std::string yamlPath;

extern std::mutex onlineListMutex;
extern std::mutex fdSocketsMutex;

ProcessBusiness::ProcessBusiness(int fd, EpollEngine *en) : _fd(fd), en(en) {
}
bool ProcessBusiness::parseCompleteRequest(std::vector<char> &buffer, std::string &dto, int &businessType)  {
    if (buffer.size() < sizeof(int) * 2) return false;

    // 协议格式: [4字节长度] + [4字节业务类型] + [数据]
    int *header = reinterpret_cast<int*>(buffer.data());
    int dataLen = ntohl(header[0]);
    int type = ntohl(header[1]);

    if (buffer.size() < sizeof(int)*2 + dataLen) return false;

    // 提取数据
    dto.assign(buffer.begin() + sizeof(int)*2, buffer.begin() + sizeof(int)*2 + dataLen);
    businessType = type;

    // 移除已处理数据
    buffer.erase(buffer.begin(), buffer.begin() + sizeof(int)*2 + dataLen);
    return true;
}


int ProcessBusiness::processBusiness(std::string dto, int businessType, std::shared_ptr<SockInfo> info) {
    LOG_INFO("business type : " << businessType);
    YAML::Node node = YAML::LoadFile(yamlPath);
    if (businessType == SSDTO::BusinessType::LOGIN_CHECK) {
        UserService uService;
        SSDTO::LoginCheckDTO ldto;
        ldto.ParseFromString(dto);

        bool isPass = uService.loginCheck({ldto.ssid(),"-1",ldto.password()});
        ldto.set_is_pass(isPass);

        string resDto;
        ldto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::LOGIN_CHECK);

        if(isPass){
            // 如果在线则顶掉
            auto isOnline = onlineList.find(ldto.ssid());
            if (isOnline != onlineList.end()) {
                std::lock_guard<std::mutex> lock(onlineListMutex);
                onlineList.erase(ldto.ssid());
                LOG("account[" << ldto.ssid() << "] is take over in another IP");
            }
            // 加入在线列表
            onlineList[ldto.ssid()] = info.get();
            // 维护SocketInfo
            fd_sockets[_fd]->ssid = ldto.ssid();
            auto resBusiness = businessSent.find(ldto.ssid());
            if(resBusiness != businessSent.end()){ // 存在未发送的业务
                for(const auto& it : resBusiness->second){
                    onlineList[ldto.ssid()]->tcp->sendMsg(it.first,it.second);
                }
                businessSent.erase(resBusiness);
            }
        }
    }
    // 消息转发
    else if (businessType == SSDTO::BusinessType::C_MESSAGE_CONTENT) {
        MessageService mService;
        SSDTO::MessageContentDTO mdto;
        mdto.ParseFromString(dto);

        // 持久化
        MessageContentDTO serverMsgDto{
            -1,mdto.sender_ssid(),static_cast<uint8_t>(mdto.content_type()),mdto.content(),{},
            {-1,mdto.recipient().message_id(),static_cast<uint8_t>(mdto.recipient().recipient_type()),
                mdto.recipient().recipient_ssid(),mdto.recipient().read_status()},mdto.create_time()
        };
        std::vector<std::string> files;
        for (const auto& it : mdto.file_id()) {
            files.push_back(it);
        }
        serverMsgDto.fileId = files;

        mService.insertUserMessage(serverMsgDto);

        // 在线转发
        std::string targetSSID = mdto.recipient().recipient_ssid();
        std::string senderSSID = mdto.sender_ssid();
        // 群组遍历在线依次转发
        if (mdto.recipient().recipient_type() == 2) {
            GroupMemberDAO gDAO;
            vector<GroupMemberInfoDO> members = gDAO.listMembers(targetSSID);
            for (const auto& member : members ) {
                if(info->ssid == member.ssidMember)continue;
                auto targetSockInfo = onlineList.find(member.ssidMember);
                if (targetSockInfo != onlineList.end()) {
                    targetSockInfo->second->tcp->sendMsg(dto,SSDTO::C_MESSAGE_CONTENT);
                }
            }
        }else { // 非群组单次转发
            auto targetSockInfo = onlineList.find(targetSSID);
            if (targetSockInfo != onlineList.end()) {
                targetSockInfo->second->tcp->sendMsg(dto,SSDTO::C_MESSAGE_CONTENT);
            }
        }
    }
    // 获取云消息
    else if (businessType == SSDTO::BusinessType::R_MESSAGE_CONTENT) {
        MessageService mService;
        SSDTO::GetUserMessageDTO mdto;
        mdto.ParseFromString(dto);

        vector<MessageContentDTO> msgRes = mService.getUserAllMessages({
            .senderSsid = mdto.ssid(),
            .createTime = static_cast<time_t>(std::stoll(mdto.last_time())),
            .pageSize = mdto.page_size(),
            .pageNum = mdto.page_num()
        });

        for (const auto& it : msgRes ) {
            SSDTO::MessageContentDTO * msgDto = mdto.add_msg();
            for (const auto& file : it.fileId) {
                msgDto->add_file_id(file);
            }
            msgDto->set_id(it.id);
            msgDto->set_sender_ssid(it.senderSsid);
            msgDto->set_content(it.content);

            SSDTO::MessageRecipientDTO * recipientDto = msgDto->mutable_recipient();
            recipientDto->set_id(it.recipient.id);
            recipientDto->set_message_id(it.recipient.messageId);
            recipientDto->set_recipient_type(it.recipient.recipientType);
            recipientDto->set_recipient_ssid(it.recipient.recipientSsid);
            recipientDto->set_read_status(it.recipient.readStatus);
            msgDto->set_create_time(it.createTime);
        }

        std::string resDto;
        mdto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::R_MESSAGE_CONTENT);
    }
    // 获取好友列表
    else if (businessType == SSDTO::BusinessType::R_FRIENDSHIP_LIST) {
        FriendshipService fService;
        FileService fileService;
        SSDTO::GetAllUserFriendship fdto;
        fdto.ParseFromString(dto);

        // 好友
        vector<FriendshipDTO>      resFriendshipInfo;
        vector<UserBaseInfoDTO>    resUserBaseInfo;

        // 群聊
        vector<GroupBaseInfoDTO>   resGroupBaseInfo;
        vector<UserBaseInfoDTO>    resGroupMemberBaseInfo;
        fService.getAllFriendshipBySSID(
            fdto.ssid(),
            resFriendshipInfo,
            resUserBaseInfo,
            resGroupBaseInfo,
            resGroupMemberBaseInfo);

        for (const auto& it : resUserBaseInfo) {
            SSDTO::UserBaseInfoDTO * udto = fdto.add_user_base_info();
            udto->set_ssid(it.ssid);
            udto->set_ssname(it.ssname);
            FileStorageDTO fileInfo = fileService.getFileByFilePath(it.avatarPath);
            if (fileInfo.fileId != "-1" && !fileInfo.fileId.empty()) {
                udto->set_avatar_file_id(fileInfo.fileId);
                udto->set_avatar_remote_path(fileInfo.storagePath);
            }
            else {
                udto->set_avatar_file_id("-1");
                udto->set_avatar_remote_path("");
            }
            udto->set_sex(std::string(1,it.sex));
            udto->set_personal_sign(it.personalSign);
            udto->set_thumb_up_count(it.thumbUpCount);
            udto->set_birthday(it.birthday);
            udto->set_region(it.region);
            udto->set_create_time(it.createTime);
        }
        for (const auto& it : resGroupBaseInfo) {
            SSDTO::GroupBaseInfoDTO * gdto = fdto.add_group_base_info();
            gdto->set_ssid_group(it.ssidGroup);
            gdto->set_name(it.name);
            FileStorageDTO fileInfo = fileService.getFileByFilePath(it.avatar);
            if (fileInfo.fileId != "-1" && !fileInfo.fileId.empty()) {
                gdto->set_avatar_file_id(fileInfo.fileId);
                gdto->set_avatar_remote_path(fileInfo.storagePath);
            }
            else {
                gdto->set_avatar_file_id("-1");
                gdto->set_avatar_remote_path("");
            }
            gdto->set_create_ssid(it.createSsid);
            gdto->set_profile(it.profile);
            for (const auto& adIt : it.admins) {
                std::string *opSSID = gdto->add_admins();
                *opSSID = adIt;
            }
            gdto->set_create_time(it.createTime);

            for (const auto& mIt : resGroupMemberBaseInfo) {
                SSDTO::UserBaseInfoDTO * udto = gdto->add_members();
                udto->set_ssid(mIt.ssid);
                udto->set_ssname(mIt.ssname);
                FileStorageDTO ufileInfo = fileService.getFileByFilePath(mIt.avatarPath);
                if (ufileInfo.fileId != "-1" && !ufileInfo.fileId.empty()) {
                    udto->set_avatar_file_id(ufileInfo.fileId);
                    udto->set_avatar_remote_path(ufileInfo.storagePath);
                }
                else {
                    udto->set_avatar_file_id("-1");
                    udto->set_avatar_remote_path("");
                }
                udto->set_sex(std::string(1,mIt.sex));
                udto->set_personal_sign(mIt.personalSign);
                udto->set_thumb_up_count(mIt.thumbUpCount);
                udto->set_birthday(mIt.birthday);
                udto->set_region(mIt.region);
                udto->set_create_time(mIt.createTime);
            }
        }
        for (const auto& it : resFriendshipInfo) {
            SSDTO::FriendshipDTO * fsdto = fdto.add_friendship_info();
            fsdto->set_id(it.id);
            fsdto->set_ssid(it.ssid);
            fsdto->set_grouping(it.grouping);
            fsdto->set_remark(it.remark);
            fsdto->set_friend_ssid(it.friendSsid);
            fsdto->set_ship_status(it.shipStatus);
            fsdto->set_friend_type(it.friendType);
            fsdto->set_create_time(it.createTime);
        }
        std::string resDto;
        fdto.SerializeToString(&resDto);

        info->tcp->sendMsg(resDto, SSDTO::BusinessType::R_FRIENDSHIP_LIST);
    }
    // 更新好友关系
    else if (businessType == SSDTO::BusinessType::U_FRIENDSHIP) {
        FriendshipService fService;
        SSDTO::FriendshipDTO fdto;
        fdto.ParseFromString(dto);
        fService.updateFriendship({
            -1,fdto.ssid(),fdto.grouping(),fdto.remark(),
            fdto.friend_ssid(),
            static_cast<uint8_t>(fdto.ship_status()),
            static_cast<uint8_t>(fdto.friend_type()),
            -1
        });
    }
    // 连接断开
    else if (businessType == SSDTO::BusinessType::DISCONNECT) {
        // 发生错误
        LOG_INFO("client( " << _fd << " , " << info->ssid << " ) disconnected.")

        // 从在线列表移除
        if(!info->ssid.empty()) {
            onlineList.erase(info->ssid);
        }
        return -1;
    }
    // 获取邮件验证码
    else if (businessType == SSDTO::BusinessType::EMAIL_VERIFY){
        DoEmailCode doEmailCode;
        info->tcp->sendMsg(doEmailCode.sendEmailCode(dto),SSDTO::BusinessType::EMAIL_VERIFY);
    }
    // 账号注册
    else if (businessType == SSDTO::BusinessType::ENROLL_ACCOUNT){
        UserService uService;
        SSDTO::EnrollAccountDTO edto;
        edto.ParseFromString(dto);

        std::string newSSID = uService.enrollAcc({"",edto.email(),edto.password(),edto.password_salt()});
        edto.set_ssid(newSSID);

        std::string resDto;
        edto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::ENROLL_ACCOUNT);
    }
    // 找回密码
    else if (businessType == SSDTO::BusinessType::RECOVER_PASSWORD){
        SSDTO::RecoverPasswordDTO edto;
        edto.ParseFromString(dto);

        UserService uService;
        uService.updateUserPrivateInfo({edto.ssid(),"-1",edto.new_password(),edto.password_salt()});
    }
    // 添加好友
    else if (businessType == SSDTO::BusinessType::MAKE_FRIEND_REQUEST){
        std::string targetSSID;
        SSDTO::MakeFriendDTO mdto;
        mdto.ParseFromString(dto);
        if (mdto.sender() != info->ssid) { // 好友请求回应
            targetSSID = mdto.sender();
            if (mdto.accept()) {           // 持久化并将 friendship list 转发给在线用户
                FriendshipService fService;
                GroupService gService;
                uint8_t friendType = (mdto.isgroup()?2:1);
                std::string grouping = mdto.isgroup()?"我加入的群聊":u8"我的好友";
                if (!mdto.isgroup()) {
                    if (fService.addFriendship(
                        {-1,mdto.sender(),grouping,"",
                            mdto.recipient(),1,friendType}))
                    {
                        FileService fFileService;
                        UserService uService;
                        SSDTO::NewFriendInfoDTO newFriendInfoSender;
                        SSDTO::NewFriendInfoDTO newFriendInfoRecipient;

                        newFriendInfoSender.set_isgroup(false);
                        newFriendInfoRecipient.set_isgroup(false);
                        newFriendInfoSender.set_grouping(grouping);
                        newFriendInfoRecipient.set_grouping(grouping);

                        auto userSenderInfo = uService.getUserInfo(mdto.sender());
                        auto userRecipientInfo = uService.getUserInfo(mdto.recipient());

                        auto toSender = newFriendInfoSender.mutable_user_info();
                        auto toRecipient = newFriendInfoRecipient.mutable_user_info();
                        if (!userSenderInfo.ssid.empty() && userSenderInfo.ssid != "-1") {
                            toSender->set_ssid(userSenderInfo.ssid);
                            toSender->set_ssname(userSenderInfo.ssname);

                            FileStorageDTO fileInfo = fFileService.getFileByFilePath(userSenderInfo.avatarPath);
                            if (fileInfo.fileId != "-1" && !fileInfo.fileId.empty()) {
                                toSender->set_avatar_file_id(fileInfo.fileId);
                                toSender->set_avatar_remote_path(fileInfo.storagePath);
                            }
                            else {
                                toSender->set_avatar_file_id("-1");
                                toSender->set_avatar_remote_path("");
                            }
                            toSender->set_sex(std::string(1,userSenderInfo.sex));
                            toSender->set_personal_sign(userSenderInfo.personalSign);
                            toSender->set_thumb_up_count(userSenderInfo.thumbUpCount);
                            toSender->set_birthday(userSenderInfo.birthday);
                            toSender->set_region(userSenderInfo.region);
                            toSender->set_create_time(userSenderInfo.createTime);

                            std::string resDto;
                            newFriendInfoSender.SerializeToString(&resDto);

                            info->tcp->sendMsg(resDto, SSDTO::BusinessType::C_FRIENDSHIP);
                        }

                        if (!userRecipientInfo.ssid.empty() && userRecipientInfo.ssid != "-1") {
                            toRecipient->set_ssid(userRecipientInfo.ssid);
                            toRecipient->set_ssname(userRecipientInfo.ssname);

                            FileStorageDTO fileInfo = fFileService.getFileByFilePath(userRecipientInfo.avatarPath);
                            if (fileInfo.fileId != "-1" && !fileInfo.fileId.empty()) {
                                toRecipient->set_avatar_file_id(fileInfo.fileId);
                                toRecipient->set_avatar_remote_path(fileInfo.storagePath);
                            }
                            else {
                                toRecipient->set_avatar_file_id("-1");
                                toRecipient->set_avatar_remote_path("");
                            }
                            toRecipient->set_sex(std::string(1,userRecipientInfo.sex));
                            toRecipient->set_personal_sign(userRecipientInfo.personalSign);
                            toRecipient->set_thumb_up_count(userRecipientInfo.thumbUpCount);
                            toRecipient->set_birthday(userRecipientInfo.birthday);
                            toRecipient->set_region(userRecipientInfo.region);
                            toRecipient->set_create_time(userRecipientInfo.createTime);

                            std::string resDto;
                            newFriendInfoRecipient.SerializeToString(&resDto);

                            auto isOnline = onlineList.find(mdto.sender());
                            if (isOnline != onlineList.end()) {
                                isOnline->second->tcp->sendMsg(resDto, SSDTO::BusinessType::C_FRIENDSHIP);
                            }
                        }
                    }
                }
                else {
                    // if (gService.(
                    //                         {-1,mdto.sender(),grouping,"",
                    //                             mdto.recipient(),1,friendType}))
                    // {
                    //     FileService fFileService;
                    //     SSDTO::NewFriendInfoDTO newFriendInfo;
                    //     newFriendInfo.set_isgroup(mdto.isgroup());
                    //     newFriendInfo.set_grouping(grouping);
                    //
                    //     fService.get
                    //     auto groupInfo = newFriendInfo.mutable_group_info();
                    //     groupInfo->set_id()
                    // }
                }
            }
        }else {                            // 向 recipient 好友申请
            dto.clear();
            mdto.set_accept(false);
            mdto.SerializeToString(&dto);
            targetSSID = mdto.recipient();
        }
        auto targetSockInfo = onlineList.find(targetSSID);
        if (targetSockInfo != onlineList.end()) {
            targetSockInfo->second->tcp->sendMsg(dto,SSDTO::MAKE_FRIEND_RESPONSE);
        }else {
            auto resDequeue = businessSent.find(targetSSID);
            if (resDequeue != businessSent.end()) {
                resDequeue->second.push_back({dto,SSDTO::MAKE_FRIEND_RESPONSE});
            }else {
                businessSent[targetSSID] = {{dto,SSDTO::MAKE_FRIEND_RESPONSE}};
            }
        }
    }
    // 搜索好友
    else if (businessType == SSDTO::BusinessType::FUZZY_SEARCH){
        UserService uService;
        GroupService gService;
        SSDTO::FuzzySearchDTO fdto;
        fdto.ParseFromString(dto);

        FileService fService;

        if (!fdto.is_group()) {
            for (const auto& it : uService.fuzzyMatch(fdto.ssid(),fdto.name())) {
                SSDTO::UserBaseInfoDTO * udto = fdto.add_user_infos();
                udto->set_ssid(it.ssid);
                udto->set_ssname(it.ssname);
                FileStorageDTO fileInfo = fService.getFileByFilePath(it.avatarPath);
                if (fileInfo.fileId != "-1" && !fileInfo.fileId.empty()) {
                    udto->set_avatar_file_id(fileInfo.fileId);
                    udto->set_avatar_remote_path(fileInfo.storagePath);
                }
                else {
                    udto->set_avatar_file_id("-1");
                    udto->set_avatar_remote_path("");
                }
                udto->set_sex(std::string(1,it.sex));
                udto->set_personal_sign(it.personalSign);
                udto->set_thumb_up_count(it.thumbUpCount);
                udto->set_birthday(it.birthday);
                udto->set_region(it.region);
                udto->set_create_time(it.createTime);
            }
        }else {
            for (const auto& it : gService.fuzzyMatch(fdto.ssid(),fdto.name())) {
                SSDTO::GroupBaseInfoDTO * gdto = fdto.add_group_infos();
                gdto->set_ssid_group(it.ssidGroup);
                gdto->set_name(it.name);
                FileStorageDTO fileInfo = fService.getFileByFilePath(it.avatar);
                if (fileInfo.fileId != "-1" && !fileInfo.fileId.empty()) {
                    gdto->set_avatar_file_id(fileInfo.fileId);
                    gdto->set_avatar_remote_path(fileInfo.storagePath);
                }
                else {
                    gdto->set_avatar_file_id("-1");
                    gdto->set_avatar_remote_path("");
                }
                gdto->set_create_ssid(it.createSsid);
                gdto->set_profile(it.profile);
                for (auto ad : it.admins)
                    gdto->add_admins(ad);
                gdto->set_create_time(it.createTime);
            }
        }
        std::string resDto;
        fdto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::FUZZY_SEARCH);
    }
    // 获取用户文件信息
    else if (businessType == SSDTO::BusinessType::R_ALL_FILE) {
        FileService fService;
        SSDTO::GetAllFileDTO fdto;
        fdto.ParseFromString(dto);

        vector<FileStorageDTO> files;

        if (fdto.ssid() != "-1" && !fdto.ssid().empty()) {
            files = fService.getFileByUserSSID(fdto.ssid(),fdto.page_size(),fdto.page_num());
        }else if (fdto.file_name() != "-1" && !fdto.file_name().empty()) {
            files = fService.getFileByFileName(fdto.file_name(),fdto.page_size(),fdto.page_num());
        }else if (fdto.file_id() != "-1" && !fdto.file_id().empty()) {
            files.push_back(fService.getFileByFileID(fdto.file_id()));
        }

        for (const auto& it : files) {
            SSDTO::FileStorageDTO * fileDto = fdto.add_files();
            fileDto->set_file_id(it.fileId);
            fileDto->set_uploader_ssid(it.uploaderSsid);
            fileDto->set_file_name(it.fileName);
            fileDto->set_file_size(it.fileSize);
            fileDto->set_file_type(it.fileType);
            fileDto->set_storage_path(it.storagePath);
            fileDto->set_upload_time(it.uploadTime);
        }

        std::string resDto;
        fdto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::R_ALL_FILE);
    }
    // 添加文件信息 by grpc sub process
    else if (businessType == SSDTO::BusinessType::C_FILE) {
        FileService fService;
        SSDTO::FileStorageDTO fdto;
        fdto.ParseFromString(dto);

        if (!fService.addFile({
            fdto.file_id(),fdto.uploader_ssid(),fdto.file_name(),
            fdto.file_size(),fdto.file_type(),fdto.storage_path(),
            fdto.upload_time()
        }))
        {
            LOG_ERROR("Failed to add file << id : " << fdto.file_id()
                << "<< ssid : " << fdto.uploader_ssid()
                << "<< file_name : " << fdto.file_name());
        }
    }
    // 修改用户基础信息
    else if (businessType == SSDTO::BusinessType::U_USER_BASE_INFO) {
        UserService uService;
        SSDTO::UserBaseInfoDTO udto;
        udto.ParseFromString(dto);

        if (!uService.updateUserBaseInfo({
            udto.ssid(),udto.ssname(),udto.avatar_remote_path(),(udto.sex().empty()?'\0':((udto.sex()=="男生")?'M':'F')),
            udto.personal_sign(),udto.thumb_up_count(),udto.birthday(),static_cast<uint8_t>(udto.region())
        }))
        {
            LOG_ERROR(
                "update user info failed << ssid : " << udto.ssid() <<
                " << ssname : " << udto.ssname() <<
                " << avatarPath : " << udto.avatar_remote_path() <<
                " << sex : " << udto.sex() <<
                " << personal_sign : " << udto.personal_sign() <<
                " << thumb_up_count : " << udto.thumb_up_count() <<
                " << birthday : " << udto.birthday() <<
                " << region : " << udto.region()
            )
        }
    }
    // 修改群组基础信息
    else if (businessType == SSDTO::BusinessType::U_GROUP_BASE_INF0) {
        GroupService gService;
        SSDTO::GroupBaseInfoDTO gdto;
        gdto.ParseFromString(dto);

        vector<string> admins;
        for (const auto &it : gdto.admins()) {
            admins.push_back(it);
        }
        FileService fService;
        FileStorageDTO fileInfo = fService.getFileByFileID(gdto.avatar_file_id());

        if (!gService.updateGroup({
            -1,gdto.ssid_group(),gdto.name(),fileInfo.storagePath,gdto.create_ssid(),gdto.profile(),
            admins
        }))
        {
            LOG_ERROR(
                "update group info failed << g_ssid : " << gdto.ssid_group() <<
                " << g_name : " << gdto.name() <<
                " << create_ssid : " << gdto.create_ssid() <<
                " << profile : " << gdto.profile()
            )
        }
    }
    // 获取用户基础信息
    else if (businessType == SSDTO::BusinessType::R_USER_BASE_INFO) {
        UserService uService;
        SSDTO::UserBaseInfoDTO udto;
        udto.ParseFromString(dto);

        UserBaseInfoDTO userInfo = uService.getUserInfo(udto.ssid());

        FileService fService;
        FileStorageDTO fileInfo = fService.getFileByFilePath(userInfo.avatarPath);
        if (!userInfo.ssid.empty()) {
            udto.set_ssname(userInfo.ssname);
            if (fileInfo.fileId != "-1" && !fileInfo.fileId.empty()) {
                udto.set_avatar_file_id(fileInfo.fileId);
                udto.set_avatar_remote_path(fileInfo.storagePath);
            }
            else {
                udto.set_avatar_file_id("-1");
                udto.set_avatar_remote_path("");
            }
            udto.set_sex(std::string(1,userInfo.sex));
            udto.set_personal_sign(userInfo.personalSign);
            udto.set_thumb_up_count(userInfo.thumbUpCount);
            udto.set_birthday(userInfo.birthday);
            udto.set_region(userInfo.region);
            udto.set_create_time(userInfo.createTime);
        }

        std::string resDto;
        udto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::R_USER_BASE_INFO);
    }
    // 获取消息图片数据
    else if (businessType == SSDTO::BusinessType::R_MESSAGE_PIC_INFO) {
        SSDTO::GetMessagePicInfoDTO gmpdto;
        gmpdto.ParseFromString(dto);
        FileService fService;

        auto _map = gmpdto.mutable_pic_name_to_path();
        for (const auto& fileInfo : (*_map)) {
            string path = fService.getFileByFileID(fileInfo.first).storagePath;
            if (path.empty()) {
                LOG_WARNING("database haven't [" <<fileInfo.first <<"] file path")
            }
            (*_map)[fileInfo.first] = path;
        }

        string resDto;
        gmpdto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::R_MESSAGE_PIC_INFO);
    }
    // video call request
    else if (businessType == SSDTO::BusinessType::VIDEO_CALL_REQUEST) {
        SSDTO::VideoCallDTO vcdto;
        vcdto.ParseFromString(dto);

        int appID = node["trtc-api"]["sdkAppId"].as<int>();
        std::string key = node["trtc-api"]["sdkSecretKey"].as<std::string>();
        std::string senderSig = GenerateUserSig::genUserSig(
                vcdto.sender_ssid().c_str(),
                appID,
                key.c_str()
            );

        std::string targetSig = GenerateUserSig::genUserSig(
                vcdto.target_ssid().c_str(),
                appID,
                key.c_str()
            );
        vcdto.set_user_sig(senderSig.c_str());

        std::string resDto;
        vcdto.SerializeToString(&resDto);
        // send response to cur user
        info->tcp->sendMsg(resDto,SSDTO::VIDEO_CALL_RESPONSE);

        // send to target
        resDto.clear();
        vcdto.set_user_sig(targetSig.c_str());
        vcdto.SerializeToString(&resDto);

        std::string targetSSID = vcdto.target_ssid();
        auto targetSockInfo = onlineList.find(targetSSID);
        if (targetSockInfo != onlineList.end()) {
            targetSockInfo->second->tcp->sendMsg(resDto,SSDTO::VIDEO_CALL_RESPONSE);
        }else {
            auto resDequeue = businessSent.find(targetSSID);
            if (resDequeue != businessSent.end()) {
                resDequeue->second.push_back({resDto,SSDTO::VIDEO_CALL_RESPONSE});
            }else {
                businessSent[targetSSID] = {{resDto,SSDTO::VIDEO_CALL_RESPONSE}};
            }
        }
    }
    // create group request
    else if (businessType == SSDTO::BusinessType::C_GROUP_BASE_INFO) {
        SSDTO::GroupBaseInfoDTO gbDto;
        gbDto.ParseFromString(dto);

        FriendshipService fService;

        GroupBaseInfoDTO baseInfo;
        std::vector<GroupMemberInfoDTO> members;
        baseInfo.createSsid = gbDto.create_ssid();
        for (const auto& admin : gbDto.admins()) {
            baseInfo.admins.push_back(admin);
        }
        for (const auto& member : gbDto.members()) {
            members.push_back({
                -1,
                "-1",
                member.ssid(),
            });
        }

        GroupService gService;

        std::string curGroupSSID = gService.createGroup(baseInfo,members);

        // 建立关系
        for (const auto& member : gbDto.members()) {
            std::string grouping = "";
            if (gbDto.create_ssid() == member.ssid()) {
                grouping = "我创建的群聊";
            }else {
                grouping = "我加入的群聊";
            }
            fService.addFriendship({
                -1,
                member.ssid(),
                grouping,
                "",
                curGroupSSID,
                1,
                2,
                gbDto.create_time()
            });
        }
        gbDto.set_name(curGroupSSID);
        gbDto.set_ssid_group(curGroupSSID);
        std::string resDto;
        gbDto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::C_GROUP_BASE_INFO);
    }
    else {
        LOG("some error occur in parse business!")
    }
    return 0;
}

