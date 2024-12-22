//
// Created by FU-QAQ on 2024/12/7.
//

#ifndef SYNERGYSPOT_STRATEGYMANAGER_H
#define SYNERGYSPOT_STRATEGYMANAGER_H
#include <QPluginLoader>
#include <QDir>
#include "define.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_PLUGIN_MANAGER_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_PLUGIN_MANAGER_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define g_pPluginManager PluginManager::getInstance()

class PluginManager : public QObject {
    Q_OBJECT
public:
    SS_API static PluginManager* getInstance();

    SS_API void unloadPlugin() {
        if(!_pluginsMap.isEmpty()){
            qDeleteAll(_pluginsMap);
        }
    }

    SS_API bool loadPlugin(const QString &pluginName,const QString &pluginPath);

    SS_API SS_Plugin* getCurrentPlugin(const QString &pluginName) const {
        auto res = _pluginsMap.find(pluginName);
        if(res != _pluginsMap.end()){
            return *res;
        }
        return nullptr;
    }

private:
    PluginManager(QObject* parent = nullptr) : QObject(parent), _pluginsMap(QMap<QString,SS_Plugin*>()) {}
    ~PluginManager(){
        unloadPlugin();
    }
private:
    static PluginManager * obj;
    QMap<QString, SS_Plugin*> _pluginsMap;
};

#endif//SYNERGYSPOT_STRATEGYMANAGER_H
