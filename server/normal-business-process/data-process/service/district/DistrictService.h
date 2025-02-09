//
// Created by FUQAQ on 2025/2/6.
//

#ifndef DISTRICTSERVICE_H
#define DISTRICTSERVICE_H

#include "../../dao/district/DistrictDAO.h"
#include "../../dto/DataProcessDTO.h"

class DistrictService {
public:
    // get province
    std::vector<std::string> getAllProvince();

    // get city
    std::vector<std::string> getAllCityInProvince(const std::string& province);

    // get district in city
    std::vector<std::string> getAllDistrictInCity(const std::string& city);

    // get name by pid
    std::string getDistrictName(uint16_t pid);

    // get pid by name
    uint16_t getDistrictPID(const std::string& name);

private:
    DistrictDAO districtDAO;
};



#endif //DISTRICTSERVICE_H
