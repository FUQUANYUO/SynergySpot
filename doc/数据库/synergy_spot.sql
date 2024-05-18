DROP DATABASE IF EXISTS `synergy_spot`;
CREATE DATABASE `synergy_spot` CHARACTER SET `utf8mb4` COLLATE `utf8mb4_0900_ai_ci`;
USE `synergy_spot`;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for user_base_info
-- ----------------------------
DROP TABLE IF EXISTS `user_base_info`;
CREATE TABLE `user_base_info` (
    `ssid` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT 'ss号',
    `ssname` VARCHAR(40) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT 'ss昵称',
    `sex` char(1) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '性别（M 男,F 女）',
    `personal_sign` VARCHAR(160) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT '' COMMENT '个性签名',
    `birthday` DATE NOT NULL COMMENT '出生日期',
    `region` SMALLINT NOT NULL DEFAULT 2 COMMENT '地区',
    PRIMARY KEY (`ssid`) USING BTREE
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '用户基本信息' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for user_private_info
-- ----------------------------
DROP TABLE IF EXISTS `user_private_info`;
CREATE TABLE `user_private_info`(
    `ssid` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT 'ss号',
    `status` VARCHAR(10) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '在线状态（off 离线，on 在线）',
    `email` VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '验证邮箱',
    `password` VARCHAR(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '用户密码',
    PRIMARY KEY (`ssid`) USING BTREE
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '用户私有信息' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for friend_ship
-- ----------------------------
DROP TABLE IF EXISTS `friendship`;
CREATE TABLE `friendship` (
    `id` INT AUTO_INCREMENT,
    `ssid` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT 'ss号',
    `friend_ssid` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '好友的ss号',
    `ship_status` TINYINT NOT NULL COMMENT '关系状态(1正常)',
    `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '添加时间',
    PRIMARY KEY (`id`) USING BTREE ,
    INDEX `ssid_index`(`ssid` ASC) USING BTREE
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '好友关系表' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for group_base_info
-- ----------------------------
DROP TABLE IF EXISTS `group_base_info`;
CREATE TABLE `group_base_info` (
    `ssid_group` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT 'ss群号',
    `name` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '群名',
    `create_ssid` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '群主ss号',
    `op_ssid` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '管理员ss号',
    `profile` VARCHAR(80) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT '' COMMENT '群简介',
    `notice` VARCHAR(400) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT '' COMMENT '群公告',
    `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    PRIMARY KEY (`ssid_group`) USING BTREE
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '群组基本信息' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for group_member_info
-- ----------------------------
DROP TABLE IF EXISTS `group_member_info`;
CREATE TABLE `group_member_info` (
    `id` INT AUTO_INCREMENT,
    `ssid_group` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT 'ss群号',
    `ssid_member` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '群员ssid',
    `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '加入时间',
    PRIMARY KEY (`id`) USING BTREE,
    INDEX `ssid_group_index`(`ssid_group` ASC) USING BTREE
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '群成员信息' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for user_contact_info
-- ----------------------------
DROP TABLE IF EXISTS `user_contact_info`;
CREATE TABLE `user_contact_info` (
    `id` INT AUTO_INCREMENT,
    `ssid` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT 'ss号',
    `friendship_id` INT COMMENT '好友关系表外键',
    `friendship_remark` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT '关系备注',
    `group_id` INT COMMENT '群组关系表外键',
    `group_remark` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT '群组备注',
    `user_name_in_group` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT '群昵称',
    `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    PRIMARY KEY (`id`) USING BTREE,
    INDEX `ssid`(`ssid` ASC) USING BTREE,
    INDEX `friendship_id_index`(`friendship_id` ASC) USING BTREE,
    INDEX `group_id_index`(`group_id` ASC) USING BTREE
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '用户联系人信息' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Table structure for user_message
-- ----------------------------
DROP TABLE IF EXISTS `user_message`;
CREATE TABLE `user_message` (
    `id` INT AUTO_INCREMENT,
    `friendship_id` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT '好友关系表外键',
    `group_member_id` VARCHAR(20) CHARACTER SET utf8 COLLATE utf8_general_ci COMMENT '群成员关系表外键',
    `content` VARCHAR(9000) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL COMMENT '消息内容',
    `create_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    PRIMARY KEY (`id`) USING BTREE,
    INDEX `friendship_id_index`(`friendship_id` ASC) USING BTREE,
    INDEX `group_member_id_index`(`group_member_id` ASC) USING BTREE
)ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '消息内容' ROW_FORMAT = DYNAMIC;

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
-- TRIGGER FOR base and private table
-- ----------------------------
DROP TRIGGER IF EXISTS `after_insert_user_private_info`;
CREATE TRIGGER `after_insert_user_private_info`
    BEFORE INSERT ON `user_private_info`
    FOR EACH ROW
    BEGIN
        INSERT INTO `user_base_info` (ssid, ssname, sex, birthday, region)
            VALUES (NEW.ssid, CONCAT('用户',NEW.ssid), 'F', TIME(NOW()), 2);
    END
;

-- ----------------------------
-- TRIGGER FOR contact and friendship or group_member
-- ----------------------------
DROP TRIGGER IF EXISTS `after_insert_friendship`;
CREATE TRIGGER `after_insert_friendship`
    AFTER INSERT ON `friendship`
    FOR EACH ROW
    BEGIN
        IF NOT EXISTS (
            SELECT 1 FROM `user_contact_info` WHERE ssid = NEW.ssid AND friendship_id = NEW.id
        ) THEN
            INSERT INTO `user_contact_info` (ssid, friendship_id, friendship_remark)
                VALUES (NEW.ssid, NEW.id, '');
        END IF;
    END
;

DROP TRIGGER IF EXISTS `after_insert_group_member`;
CREATE TRIGGER `after_insert_group_member`
    AFTER INSERT ON `group_member_info`
    FOR EACH ROW
    BEGIN
        INSERT INTO `user_contact_info` (ssid, group_id, group_remark, user_name_in_group)
            VALUES (NEW.ssid_member, NEW.id, '', '');
    END
;
