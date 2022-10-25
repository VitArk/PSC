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
// Created by Vitalii Arkusha on 12.05.2021.
//

#ifndef POWER_SUPPLY_CONTROLLER_APPLICATION_H
#define POWER_SUPPLY_CONTROLLER_APPLICATION_H

#include <QObject>
#include <QApplication>
#include <QQueue>
#include "protocol/Commons.h"
#include "Communication.h"
#include "MainWindow.h"

class Application : public QApplication {
    Q_DISABLE_COPY(Application)
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv, int = ApplicationFlags);
    ~Application() override;

private:
    Communication *mCommunication;
    MainWindow *mMainWindow;
    QTimer     mWorkingTimer;

private slots:
    void slotAppRun();

    void slotStartWorking(DeviceInfo info);
    void slotSerialPortClosed();

    void slotWorkingCycle();
    void slotOutputStatus(DeviceStatus status);
    void slotOutputProtectionChanged(TOutputProtection protection);
    void slotTuneWorkingTimerInterval(const CommunicationMetrics &metrics);
};


#endif //POWER_SUPPLY_CONTROLLER_APPLICATION_H
