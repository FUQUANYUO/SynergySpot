//
// Created by FU-QAQ on 2025/5/20.
//

#ifndef PROTOCONTENT_H
#define PROTOCONTENT_H

#include <iostream>

const std::string privacyContent = R"(
<!-- im_privacy_policy.html -->
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <style>
        body {
            font-family: 'Arial', 'Microsoft YaHei', sans-serif;
            line-height: 1.6;
            padding: 20px;
            color: #333;
        }
        .security-badge {
            background: #e3f2fd;
            padding: 15px;
            border-radius: 5px;
            margin: 20px 0;
        }
    </style>
</head>
<body>
    <h1>即时通讯软件隐私政策</h1>
    <div class="update-date">生效日期：2025年5月 | 版本：1.0</div>

    <div class="security-badge">
        🔒 我们采用端到端加密技术保护您的通信安全
    </div>

    <h2>1. 我们收集的信息</h2>
    <p>为提供即时通讯服务，我们可能收集：</p>
    <p>• 账户信息：手机号/邮箱、用户名、头像<br>
       • 通信内容：文本消息、语音/视频记录、文件传输（加密存储）<br>
       • 设备信息：设备型号、IP地址、连接日志<br>
       • 社交数据：联系人列表、群组信息、聊天偏好设置</p>

    <h2>2. 信息使用方式</h2>
    <p>• 消息路由：实现用户间的实时通信传输<br>
       • 服务优化：分析连接成功率、消息延迟等质量数据<br>
       • 安全防护：检测垃圾信息、网络攻击等异常行为<br>
       • 功能扩展：支持消息撤回、已读回执等增强功能</p>

    <h2>3. 数据共享与披露</h2>
    <p>严格限制在以下场景：<br>
       • 消息传输：通过SSL/TLS加密通道进行数据传输<br>
       • 第三方服务：推送通知（仅共享设备令牌）、云存储服务<br>
       • 法律要求：配合司法机关的合法数据调取</p>

    <h2>4. 安全防护措施</h2>
    <p>• 通信加密：所有消息使用Signal协议端到端加密<br>
       • 存储加密：用户文件采用加密存储<br>
       • 自动清理：可选设置消息自动销毁时间（24小时/7天/1个月）<br>
       • 安全认证：每年通过第三方安全审计</p>

    <h2>5. 您的权利</h2>
    <p>• 消息管理：随时删除本地聊天记录<br>
       • 账户控制：支持完整账户注销及数据清除<br>
       • 权限管理：可在设置中随时关闭相机/麦克风权限<br>
       • 数据导出：提供聊天记录导出功能（PDF/文本格式）</p>

    <h2>6. 国际数据传输</h2>
    <p>• 服务器分布：全球分布式部署<br>
       • 跨境传输：遵守GDPR数据传输标准</p>

    <h2>7. 联系我们</h2>
    <p>隐私专员邮箱：synergyspot@yourcompany.com<br>
       应急响应：安全事件报告通道（应用内「设置-安全中心」）<br>
       欧洲代表：根据GDPR第27条指定</p>
</body>
</html>
)";

const std::string serviceContent = R"(
<!-- service_agreement.html -->
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <style>
        .clause-item {
            margin: 15px 0;
            padding-left: 20px;
            border-left: 3px solid #4CAF50;
        }
        .prohibition-list {
            background: #fff3e0;
            padding: 15px;
            border-radius: 5px;
            margin: 15px 0;
        }
    </style>
</head>
<body>
    <h1>即时通讯软件服务协议</h1>
    <div class="update-info">版本：1.0 | 生效日期：2025年5月</div>

    <div class="clause-item">
        <h2>一、服务内容</h2>
        <p>1.1 本软件提供包括但不限于：<br>
           • 实时文字/语音/视频通信<br>
           • 文件传输<br>
           • 群组聊天（最大100人）<br>
           • 消息云存储</p>
    </div>

    <div class="clause-item">
        <h2>二、账号管理</h2>
        <p>2.1 用户需通过手机号或邮箱完成实名认证<br>
           2.2 账号安全责任：<br>
           • 禁止转让/出借账号<br>
           • 异常登录自动触发二次验证<br>
           • 连续6个月未登录将冻结账号</p>
    </div>

    <div class="prohibition-list">
        <h3>禁止行为</h3>
        <p>✖ 发布违反法律法规的内容<br>
           ✖ 进行垃圾信息轰炸（每分钟超过50条）<br>
           ✖ 逆向工程/破解客户端<br>
           ✖ 滥用群发功能进行商业营销</p>
    </div>

    <div class="clause-item">
        <h2>三、服务变更</h2>
        <p>3.1 重大更新将提前30天通过应用内通知公告<br>
           3.2 免费用户每日视频通话时长限制为3小时<br>
           3.3 服务器维护期间可能暂停服务（每月不超过4小时）</p>
    </div>

    <div class="clause-item">
        <h2>四、免责声明</h2>
        <p>4.1 因以下情况导致的服务中断免责：<br>
           • 不可抗力（自然灾害、政府行为等）<br>
           • 用户设备或网络故障<br>
           • 第三方服务（如运营商网络）问题</p>
    </div>

    <div class="clause-item">
        <h2>五、协议修改</h2>
        <p>修改后的协议在<a href="https://example.com/terms">官方网站</a>公布后生效，继续使用视为接受新条款</p>
    </div>
</body>
</html>
)";

#endif //PROTOCONTENT_H
