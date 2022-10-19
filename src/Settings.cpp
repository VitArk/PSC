//
// Created by Vitalii Arkusha on 30.09.2022.
//

#include "Settings.h"
#include "Application.h"
#include <QDebug>

Settings::Settings(QObject *parent) : QObject(parent),
mSettings(QSettings::Scope::UserScope,
          Application::organizationName(),
          Application::applicationName(), parent)
          {
}

int Settings::serialPortBaudRate() const {
    return mSettings.value("serial-port/baud-rate", 9600).toInt();
}

void Settings::setSerialPortBaudRate(int baudRate) {
    setValue("serial-port/baud-rate", baudRate);
}


QString Settings::serialPortName() const {
    return mSettings.value("serial-port/name").toString();
}

void Settings::setSerialPortName(QString name) {
    setValue("serial-port/name", name);
}

bool Settings::isDebugModeEnabled() const {
    return mSettings.value("debug-mode/enabled").toBool();
}

void Settings::setDebugModeEnabled(bool enabled) {
    setValue("debug-mode/enabled", enabled);
}
