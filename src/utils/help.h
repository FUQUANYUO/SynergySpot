//
// Created by FU-QAQ on 2024/2/25.
//

#ifndef SYNERGYSPOT_HELP_H
#define SYNERGYSPOT_HELP_H

#include <iostream>
#include <algorithm>

extern std::string yamlPath;
extern std::string CurSSID;
extern std::string CurSSname;

#define LOG(__STR__) \
    std::cout << "[" << __FILE__ << "] |in " << __LINE__ << " line |at " << __TIME__ << "] : " << __STR__ << std::endl;

#endif//SYNERGYSPOT_HELP_H
