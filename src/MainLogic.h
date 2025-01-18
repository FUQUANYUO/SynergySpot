//
// Created by FU-QAQ on 2024/12/10.
//

#ifndef SYNERGYSPOT_MAINLOGIC_H
#define SYNERGYSPOT_MAINLOGIC_H

#include <QObject>
#include <QApplication>

class QProcess;

class MainLogic : public QObject{
    Q_OBJECT
public:
    explicit MainLogic();
    ~MainLogic();

    int startMainLogic(QApplication * app);
private:
    QProcess * _pGRPCProcess = nullptr;
};


#endif//SYNERGYSPOT_MAINLOGIC_H
