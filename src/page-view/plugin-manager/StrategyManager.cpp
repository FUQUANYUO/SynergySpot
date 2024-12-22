//
// Created by FU-QAQ on 2024/12/8.
//
#include "StrategyManager.h"
#include "help.h"
#include <mutex>

static std::mutex m;
PluginManager * PluginManager::obj = nullptr;

PluginManager *PluginManager::getInstance() {
    if(!obj){
        m.lock();
        if(!obj){
            obj = new PluginManager;
        }
        m.unlock();
    }
    return obj;
}

bool PluginManager::loadPlugin(const QString &pluginName,const QString &pluginPath) {
    QPluginLoader loader(pluginPath);
    QObject* plugin = loader.instance();
    if (plugin) {
        auto * castPlugin = dynamic_cast<SS_Plugin*>(plugin);
        if (castPlugin) {
            _pluginsMap.insert(pluginName, castPlugin);
            return true;
        } else {
            LOG("Failed to cast plugin to IBackgroundStrategy.")
        }
    } else {
        LOG("Failed to load plugin:" << loader.errorString().toStdString())
    }
    return false;
}
