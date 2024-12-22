//
// Created by FU-QAQ on 2024/12/10.
//

#ifndef SYNERGYSPOT_MAINLOGIC_H
#define SYNERGYSPOT_MAINLOGIC_H

#include <QObject>
#include <QApplication>

class MainLogic : public QObject{
    Q_OBJECT
public:
    explicit MainLogic()  = default;
    ~MainLogic() override = default;

    int startMainLogic();
};


#endif//SYNERGYSPOT_MAINLOGIC_H
