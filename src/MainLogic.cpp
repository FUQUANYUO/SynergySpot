//
// Created by FU-QAQ on 2024/12/10.
//

#include "MainLogic.h"

#include "land-page/LandPage.h"
#include "plugin-manager/StrategyManager.h"
#include "effect-component/cv-process-video-strategy/CVProVideoStrategy.h"
#include "arch-page/ArchPage.h"

#include "ela-widget-tools/ElaApplication.h"

int MainLogic::startMainLogic() {
    eApp->init();

    // enable mica style
    eApp->setIsEnableMica(true);

    QString execPath = QApplication::applicationDirPath();

    QString BKStrategyPluginName;
#ifdef QT_DEBUG
    BKStrategyPluginName = "/EffectComponentPlugind.dll";
#else
    BKStrategyPluginName = "/EffectComponentPlugin.dll";
#endif
    // init Background strategy plugin
    if(g_pPluginManager->loadPlugin(BK_PLUGIN_NAME,execPath + BKStrategyPluginName)){
        // copy bk source from landpage rc to bin
        QDir tmpDir;
        if(!tmpDir.exists(execPath + "/assets")){
            tmpDir.mkpath(execPath + "/assets");
            //            g_pLandPage->saveQrcToFile(":/land-page/rc-page/video/bk_src.mp4",
            //                                       execPath + "/assets/bk_src.mp4");
        }

        // init effect bk source
        dynamic_cast<CVProVideoStrategy*>(g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME))->initialize(
                execPath + "/assets/bk_src.mp4"
        );
        g_pLandPage->show();

        // TODO : link to core code
        // trigger verify account request
        connect(g_pLandPage,&LandPage::sigSignInRequest,this,[=](const QString& SSID,const QString& password){
            // TODO: ignored request handler
            g_pLandPage->close();
            g_pLandPage->destroyLandPage();

            // go to arch page
            g_pArchPage->show();
        });
    }
    return QApplication::exec();
}
