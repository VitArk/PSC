//
// Created by Vitalii Arkusha on 12.05.2021.
//

#ifndef POWER_SUPPLY_CONTROLLER_APPLICATION_H
#define POWER_SUPPLY_CONTROLLER_APPLICATION_H

#include <QObject>
#include <QApplication>
#include "Commons.h"
#include "Communication.h"
#include "MainWindow.h"

#if defined(App)
#undef App
#endif
#define App (Application::i())

class Application : public QApplication {
    Q_DISABLE_COPY(Application)

Q_OBJECT
public:
    explicit Application(int &argc, char **argv, int = ApplicationFlags);

    ~Application() override;

    static Application *i() { return inst; }

public slots:


private:
    static Application *inst;
    Communication *mCommunication;
    MainWindow *mMainWindow;
    QTimer     mWorkingTimer;

private slots:
    void slotAppRun();

    void slotSerialPortOpened();
    void slotSerialPortClosed();

    void slotVoltageChanged(TChannel channel, double value);
    void slotCurrentChanged(TChannel channel, double value);

    void slotWorkingCycle();
};


#endif //POWER_SUPPLY_CONTROLLER_APPLICATION_H
