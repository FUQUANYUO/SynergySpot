-- ----------------------------
-- Create user named SSAdmin
-- ----------------------------
DROP USER IF EXISTS 'SSAdmin'@'localhost';
DROP USER IF EXISTS 'SSAdmin'@'%';
CREATE USER 'SSAdmin'@'%' IDENTIFIED BY 'SS123456';

USE mysql;
UPDATE user SET host='%' WHERE user='SSAdmin' and host='localhost';CREATE USER 'SSAdmin'@'localhost' IDENTIFIED BY 'SS123456';
GRANT ALL PRIVILEGES ON synergy_spot.* TO 'SSAdmin'@'%';
FLUSH PRIVILEGES;