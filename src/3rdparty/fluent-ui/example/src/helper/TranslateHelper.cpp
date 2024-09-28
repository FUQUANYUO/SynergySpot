#include "TranslateHelper.h"

#include <QGuiApplication>
#include <QQmlEngine>

#include "SettingsHelper.h"

[[maybe_unused]] TranslateHelper::TranslateHelper(QObject *parent) : QObject(parent) {
    _languages << "en_US";
    _languages << "zh_CN";
    _current = SettingsHelper::getInstance()->getLanguage();
}

TranslateHelper::~TranslateHelper() = default;

void TranslateHelper::init(QQmlEngine *engine) {
    _engine = engine;
    _translator = new QTranslator(this);
    QGuiApplication::installTranslator(_translator);
    QString translatorPath = QGuiApplication::applicationDirPath() + "/i18n";
    if (_translator->load(
            QString::fromStdString("%1/example_%2.qm").arg(translatorPath, _current))) {
        _engine->retranslate();
    }
}
