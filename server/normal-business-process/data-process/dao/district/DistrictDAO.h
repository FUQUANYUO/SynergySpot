//
// Created by FUQAQ on 2025/2/5.
//

#ifndef DISTRICTDAO_H
#define DISTRICTDAO_H

#include "../DataProcessDAO.h"

class DistrictDAO : public IDistrictDAO {
public:
    DistrictDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG("Failed to get MySQL connection from pool.")
        }
    }

    virtual ~DistrictDAO() = default;

    std::string getDistrictName(uint16_t pid) override;

    std::vector<DistrictDO> findByLevel(uint8_t level) override;

    uint16_t getDistrictPid(const std::string& districtName);

    std::vector<std::string> getAllProvinces();

    std::vector<std::string> getAllCitiesInProvince(uint16_t pid);

    std::vector<std::string> getAllDistrictsInCity(uint16_t pid);
private:
    std::shared_ptr<MysqlConn> m_conn;
};


#endif //DISTRICTDAO_H
