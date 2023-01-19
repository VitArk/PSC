// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//
// Created by Vitalii Arkusha on 30.09.2022.
//

#include "Settings.h"
#include "Application.h"

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
