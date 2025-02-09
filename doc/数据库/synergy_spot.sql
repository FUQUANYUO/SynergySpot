CREATE DATABASE IF NOT EXISTS `synergy_spot` CHARACTER SET `utf8mb4` COLLATE `utf8mb4_0900_ai_ci`;
USE `synergy_spot`;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;
-- ----------------------------
-- Table structure for user_base_info
-- ----------------------------
DROP TABLE IF EXISTS `user_base_info`;
CREATE TABLE `user_base_info` (
                                  `ssid` VARCHAR(20) NOT NULL COMMENT 'ss号',
                                  `ssname` VARCHAR(40) NOT NULL COMMENT 'ss昵称',
                                  `avatar` VARCHAR(255)  COMMENT '头像路径',
                                  `sex` CHAR(1) NOT NULL COMMENT '性别（M 男,F 女）',
                                  `personal_sign` VARCHAR(160) DEFAULT '' COMMENT '个性签名',
                                  `thumb_up_count` INT DEFAULT '0' COMMENT '点赞数',
                                  `birthday` DATE COMMENT '出生日期',
                                  `create_time` DATE NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
                                  `region` SMALLINT NOT NULL DEFAULT 2 COMMENT '地区',
                                  PRIMARY KEY (`ssid`),
                                  INDEX `idx_region` (`region`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '用户基本信息';


-- ----------------------------
-- Table structure for user_private_info
-- ----------------------------
DROP TABLE IF EXISTS `user_private_info`;
CREATE TABLE `user_private_info`(
                                    `ssid` VARCHAR(20) NOT NULL COMMENT 'ss号',
                                    `email` VARCHAR(50) NOT NULL COMMENT '验证邮箱',
                                    `password` VARCHAR(512) NOT NULL COMMENT '用户密码',
                                    `password_salt` VARCHAR(50) NOT NULL COMMENT '密码盐',
                                    `account_status` TINYINT DEFAULT 1 COMMENT '账号状态',
                                    PRIMARY KEY (`ssid`),
                                    INDEX `idx_email` (`email`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '用户私有信息';

-- ----------------------------
-- Table structure for friend_ship
-- ----------------------------
DROP TABLE IF EXISTS `friendship`;
CREATE TABLE `friendship` (
                              `id` INT AUTO_INCREMENT,
                              `ssid` VARCHAR(20) NOT NULL COMMENT 'ss号',
                              `grouping` VARCHAR(255) NOT NULL COMMENT '分组名',
                              `remark` VARCHAR(40) DEFAULT '' COMMENT '备注',
                              `friend_ssid` VARCHAR(20) NOT NULL COMMENT '好友的ss号',
                              `ship_status` TINYINT NOT NULL COMMENT '关系状态',
                              `friend_type` TINYINT DEFAULT 1 COMMENT '好友类型(1-好友，2-群组)',
                              `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '添加时间',
                              PRIMARY KEY (`id`),
                              UNIQUE KEY `uniq_ssid_friend` (`ssid`, `friend_ssid`),
                              INDEX `idx_friend_ssid` (`friend_ssid`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '好友关系表';

-- ----------------------------
-- Table structure for group_base_info
-- ----------------------------
DROP TABLE IF EXISTS `group_base_info`;
CREATE TABLE `group_base_info` (
                                   `id` INT AUTO_INCREMENT,
                                   `ssid_group` VARCHAR(20) NOT NULL COMMENT 'ss群号',
                                   `name` VARCHAR(20) NOT NULL COMMENT '群名',
                                   `avatar` VARCHAR(255) NOT NULL COMMENT '群头像路径',
                                   `create_ssid` VARCHAR(20) NOT NULL COMMENT '群主ss号',
                                   `profile` VARCHAR(80) DEFAULT '' COMMENT '群简介',
                                   `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
                                   PRIMARY KEY (`id`),
                                   UNIQUE KEY `uniq_ssid_group` (`ssid_group`),
                                   INDEX `idx_create_ssid` (`create_ssid`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '群组基本信息';

-- ----------------------------
-- 支持多管理员
-- ----------------------------
DROP TABLE IF EXISTS `group_admin`;
CREATE TABLE `group_admin` (
                               `id` INT AUTO_INCREMENT,
                               `group_id` INT NOT NULL COMMENT '群组ID',
                               `op_ssid` VARCHAR(20) NOT NULL COMMENT '管理员ss号',
                               PRIMARY KEY (`id`),
                               FOREIGN KEY (`group_id`) REFERENCES `group_base_info`(`id`) ON DELETE CASCADE,
                               UNIQUE KEY `uniq_group_admin` (`group_id`, `op_ssid`),
                               INDEX `idx_op_ssid` (`op_ssid`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '群管理员表';

-- ----------------------------
-- 支持多公告
-- ----------------------------
DROP TABLE IF EXISTS `group_notice`;
CREATE TABLE `group_notice` (
                                `id` INT AUTO_INCREMENT,
                                `group_id` INT NOT NULL COMMENT '群组ID',
                                `notice` VARCHAR(400) NOT NULL COMMENT '群公告',
                                `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '发布时间',
                                PRIMARY KEY (`id`),
                                FOREIGN KEY (`group_id`) REFERENCES `group_base_info`(`id`) ON DELETE CASCADE,
                                INDEX `idx_group_time` (`group_id`, `create_time`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '群公告表';

-- ----------------------------
-- Table structure for group_member_info
-- ----------------------------
DROP TABLE IF EXISTS `group_member_info`;
CREATE TABLE `group_member_info` (
                                     `id` INT AUTO_INCREMENT,
                                     `ssid_group` VARCHAR(20) NOT NULL COMMENT 'ss群号',
                                     `ssid_member` VARCHAR(20) NOT NULL COMMENT '群员ssid',
                                     `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '加入时间',
                                     PRIMARY KEY (`id`),
                                     UNIQUE KEY `uniq_group_member` (`ssid_group`, `ssid_member`),
                                     FOREIGN KEY (`ssid_group`) REFERENCES `group_base_info`(`ssid_group`) ON DELETE CASCADE,
                                     INDEX `idx_member` (`ssid_member`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '群成员信息';

-- ----------------------------
-- Table structure for message_content
-- ----------------------------
DROP TABLE IF EXISTS `message_content`;
CREATE TABLE `message_content` (
                                   `id` BIGINT AUTO_INCREMENT,
                                   `sender_ssid` VARCHAR(20) NOT NULL COMMENT '发送者ss号',
                                   `content_type` TINYINT NOT NULL COMMENT '消息类型（1文本/2文件）',
                                   `content` TEXT NOT NULL COMMENT '消息内容',
                                   `file_id` VARCHAR(50) COMMENT '关联文件ID',
                                   `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
                                   PRIMARY KEY (`id`),
                                   FOREIGN KEY (`sender_ssid`) REFERENCES `user_base_info`(`ssid`) ON DELETE CASCADE,
                                   INDEX `idx_sender` (`sender_ssid`),
                                   INDEX `idx_file` (`file_id`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '消息内容表';

-- ----------------------------
-- 消息内容表 + 接收者映射表
-- ----------------------------
DROP TABLE IF EXISTS `message_recipient`;
CREATE TABLE `message_recipient` (
                                     `id` BIGINT AUTO_INCREMENT,
                                     `message_id` BIGINT NOT NULL COMMENT '消息ID',
                                     `recipient_type` TINYINT NOT NULL COMMENT '接收者类型（1用户/2群组）',
                                     `recipient_ssid` VARCHAR(20) NOT NULL COMMENT '接收者ID',
                                     `read_status` TINYINT DEFAULT 0 COMMENT '已读状态（0未读/1已读）',
                                     PRIMARY KEY (`id`),
                                     INDEX `idx_recipient` (`recipient_ssid`, `recipient_type`),
                                     FOREIGN KEY (`message_id`) REFERENCES `message_content`(`id`) ON DELETE CASCADE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '消息接收者映射表';

-- ----------------------------
-- Table structure for rc_district
-- ----------------------------
DROP TABLE IF EXISTS `district`;
CREATE TABLE `district` (
  `district_id` smallint(5) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增id',
  `pid` smallint(5) unsigned NOT NULL DEFAULT '0' COMMENT '父及关系',
  `district` VARCHAR(120) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '地区名称',
  `level` tinyint(1) NOT NULL COMMENT '子属关系',
  PRIMARY KEY (`district_id`),
  KEY `parent_id` (`pid`),
  KEY `region_type` (`level`)
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '地区' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- 文件系统表
-- ----------------------------
DROP TABLE IF EXISTS `file_storage`;
CREATE TABLE `file_storage` (
                                `file_id` VARCHAR(50) NOT NULL COMMENT '文件唯一ID',
                                `uploader_ssid` VARCHAR(20) NOT NULL COMMENT '上传者ss号',
                                `file_name` VARCHAR(255) NOT NULL COMMENT '文件名',
                                `file_size` BIGINT NOT NULL COMMENT '文件大小（字节）',
                                `file_type` VARCHAR(50) NOT NULL COMMENT '文件类型',
                                `storage_path` VARCHAR(500) NOT NULL COMMENT '存储路径',
                                `upload_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '上传时间',
                                PRIMARY KEY (`file_id`),
                                INDEX `idx_uploader` (`uploader_ssid`),
                                INDEX `idx_file_type` (`file_type`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '文件存储表';

-- ----------------------------
-- 日志系统表
-- ----------------------------
DROP TABLE IF EXISTS `operation_log`;
CREATE TABLE `operation_log` (
                                 `id` BIGINT AUTO_INCREMENT,
                                 `ssid` VARCHAR(20) NOT NULL COMMENT '操作用户',
                                 `operation_type` VARCHAR(50) NOT NULL COMMENT '操作类型（如login/send_msg）',
                                 `detail` TEXT COMMENT '操作详情',
                                 `ip_address` VARCHAR(45) COMMENT 'IP地址',
                                 `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '操作时间',
                                 PRIMARY KEY (`id`),
                                 INDEX `idx_ssid` (`ssid`),
                                 INDEX `idx_op_type` (`operation_type`)
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COMMENT = '操作日志表';

-- ----------------------------
-- 基础表情包表
-- ----------------------------
DROP TABLE IF EXISTS `base_stickers`;
CREATE TABLE `base_stickers` (
                                 `sticker_id` INT AUTO_INCREMENT COMMENT '表情ID',
                                 `image_url` VARCHAR(255) NOT NULL COMMENT '图片URL',
                                 `tags` VARCHAR(100) COMMENT '标签（逗号分隔）',
                                 `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
                                 PRIMARY KEY (`sticker_id`),
                                 INDEX `idx_tags` (`tags`)  -- 支持标签模糊查询
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='基础表情包';

-- ----------------------------
-- 用户收藏表情表（每个用户独立一套）
-- ----------------------------
DROP TABLE IF EXISTS `user_collected_stickers`;
CREATE TABLE `user_collected_stickers` (
                                           `collection_id` INT AUTO_INCREMENT COMMENT '收藏ID',
                                           `user_ssid` VARCHAR(20) NOT NULL COMMENT '用户ID',
                                           `is_custom` INT NOT NULL DEFAULT 0 CHECK(is_custom IN (0,1)) COMMENT '是否自制',
                                           `image_url` VARCHAR(255) NOT NULL COMMENT '图片URL',
                                           `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '收藏时间',
                                           PRIMARY KEY (`collection_id`),
                                           INDEX `idx_user` (`user_ssid`),
                                           FOREIGN KEY (`user_ssid`) REFERENCES `user_base_info`(`ssid`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='用户收藏表情表';

-- ----------------------------
-- TRIGGER FOR base and private table
-- ----------------------------
DROP TRIGGER IF EXISTS `after_insert_user_private_info`;
DELIMITER //
CREATE TRIGGER `after_insert_user_private_info`
    BEFORE INSERT ON `user_private_info`
    FOR EACH ROW
    BEGIN
        INSERT INTO `user_base_info` (ssid, ssname, sex, birthday, create_time, region)
            VALUES (NEW.ssid, CONCAT('用户',NEW.ssid), 'F', CURDATE(),CURRENT_TIMESTAMP, 2);
    END;
//
DELIMITER ;

