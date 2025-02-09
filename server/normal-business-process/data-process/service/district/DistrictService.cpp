//
// Created by FUQAQ on 2025/2/6.
//

#include "DistrictService.h"

std::vector<std::string> DistrictService::getAllProvince() {
    return districtDAO.getAllProvinces();
}

std::vector<std::string> DistrictService::getAllCityInProvince(const std::string &province) {
    return districtDAO.getAllCitiesInProvince(districtDAO.getDistrictPid(province));
}

std::vector<std::string> DistrictService::getAllDistrictInCity(const std::string &city) {
    return districtDAO.getAllDistrictsInCity(districtDAO.getDistrictPid(city));
}

std::string DistrictService::getDistrictName(uint16_t pid) {
    return districtDAO.getDistrictName(pid);
}

uint16_t DistrictService::getDistrictPID(const std::string &name) {
    return districtDAO.getDistrictPid(name);
}