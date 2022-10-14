//
// Created by Vitalii Arkusha on 12.05.2021.
//

#ifndef POWER_SUPPLY_CONTROLLER_APPLICATION_H
#define POWER_SUPPLY_CONTROLLER_APPLICATION_H

#include <QObject>
#include <QApplication>
#include "devices/Commons.h"
#include "Communication.h"
#include "MainWindow.h"
#include "Settings.h"

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
    Settings   mSettings;

private slots:
    void slotAppRun();

    void slotStartWorking(DeviceInfo info);
    void slotSerialPortClosed();

    void slotWorkingCycle();
    void slotOutputStatus(DeviceStatus status);
    void slotOutputProtectionChanged(TOutputProtection protection);

};


#endif //POWER_SUPPLY_CONTROLLER_APPLICATION_H
