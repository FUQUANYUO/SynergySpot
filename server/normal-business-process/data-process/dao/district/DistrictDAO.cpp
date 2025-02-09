//
// Created by FUQAQ on 2025/2/5.
//

#include "DistrictDAO.h"

std::string DistrictDAO::getDistrictName(uint16_t pid) {
    std::string sql = "SELECT `district` FROM `district` WHERE `district_id` = ?";
    std::vector<MysqlConn::Param> params;
    MysqlConn::Param param;
    param.type = MysqlConn::Param::INT;
    param.int_val = pid;
    params.push_back(param);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to execute query: " + sql)
        return "-1";
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0]) {
        std::string name = row[0];
        mysql_free_result(result);
        return name;
    }

    mysql_free_result(result);
    LOG_ERROR("Failed to get district district name pid = " + pid);
    return "-1";
}

std::vector<DistrictDO> DistrictDAO::findByLevel(uint8_t level) {
    std::string sql = "SELECT `district_id`, `pid`, `district`, `level` FROM `district` WHERE `level` = ?";
    std::vector<MysqlConn::Param> params;
    MysqlConn::Param param;
    param.type = MysqlConn::Param::INT;
    param.int_val = level;
    params.push_back(param);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG("Failed to execute query: " + sql)
        return {};
    }

    std::vector<DistrictDO> districts;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        DistrictDO district;
        district.districtId = std::stoul(row[0]);
        district.pid = std::stoul(row[1]);
        district.district = row[2];
        district.level = std::stoul(row[3]);
        districts.push_back(district);
    }

    mysql_free_result(result);
    return districts;
}

uint16_t DistrictDAO::getDistrictPid(const std::string &districtName) {
    std::string sql = "SELECT `pid` FROM `district` WHERE `districtName` = ?";
    std::vector<MysqlConn::Param> params;
    MysqlConn::Param param;
    param.type = MysqlConn::Param::STRING;
    param.str_val = districtName;
    params.push_back(param);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to execute query: " + sql)
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row && row[0]) {
        uint16_t pid = std::stoi(row[0]);
        mysql_free_result(result);
        return pid;
    }

    mysql_free_result(result);
    LOG_ERROR("Failed to get district district name pid = " + districtName);
    return -1;
}

std::vector<std::string> DistrictDAO::getAllProvinces() {
    std::string sql = "SELECT `district` FROM `district` WHERE `level` = 1";
    MYSQL_RES* result = m_conn->query(sql, {});
    if (!result) {
        LOG_ERROR("Failed to get all provinces.");
        return {};
    }

    std::vector<std::string> provinces;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && row[0]) {
        provinces.emplace_back(row[0]);
    }
    mysql_free_result(result);
    return provinces;
}

std::vector<std::string> DistrictDAO::getAllDistrictsInCity(uint16_t pid) {
    std::string sql = "SELECT `district` FROM `district` "
                          "WHERE `pid` = ? AND `level` = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramCityId;
    paramCityId.type = MysqlConn::Param::INT;
    paramCityId.int_val = pid;
    params.push_back(paramCityId);

    MysqlConn::Param paramLevel;
    paramLevel.type = MysqlConn::Param::INT;
    paramLevel.int_val = 3;
    params.push_back(paramLevel);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to get district for city ID: " << pid);
        return {};
    }

    std::vector<std::string> districts;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && row[0]) {
        districts.emplace_back(row[0]);
    }
    mysql_free_result(result);
    return districts;

}

std::vector<std::string> DistrictDAO::getAllCitiesInProvince(uint16_t provinceId) {
    std::string sql = "SELECT `district` FROM `district` "
                      "WHERE `pid` = ? AND `level` = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramProvinceId;
    paramProvinceId.type = MysqlConn::Param::INT;
    paramProvinceId.int_val = provinceId;
    params.push_back(paramProvinceId);

    MysqlConn::Param paramLevel;
    paramLevel.type = MysqlConn::Param::INT;
    paramLevel.int_val = 2;
    params.push_back(paramLevel);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to get cities for province ID: " << provinceId);
        return {};
    }

    std::vector<std::string> cities;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && row[0]) {
        cities.emplace_back(row[0]);
    }
    mysql_free_result(result);
    return cities;
}