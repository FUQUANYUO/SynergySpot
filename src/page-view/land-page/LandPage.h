//
// Created by FU-QAQ on 2024/9/12.
//

#ifndef SYNERGYSPOT_LANDPAGE_H
#define SYNERGYSPOT_LANDPAGE_H

#include "../arch-page/ArchPage.h"

#define pLandPage LandPage::getLandPage()

class SS_API_OUT LandPage : public SSUi::ArchPageWidget {
public:
    static LandPage * getLandPage();
    static void destroyLandPage();
private:
    explicit LandPage(QWidget *parent = nullptr);
    ~LandPage() override;

    void initWindow() override;
    void initEdgeLayout() override;
    void initContent() override;
protected:
    void initConnectFunc() override;
private:
    static LandPage* _landPage;
};


#endif//SYNERGYSPOT_LANDPAGE_H
