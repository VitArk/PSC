//
// Created by Vitalii Arkusha on 12.05.2021.
//

#include "Application.h"
#include <QTimer>
#include <QDebug>

Application *Application::inst = nullptr;

Application::Application(int &argc, char **argv, int) : QApplication(argc, argv) {
    Q_ASSERT(!inst); // only one instance should be for app.
    inst = this;

    QApplication::setApplicationName("PSC");
    QApplication::setApplicationVersion("1.0");

    mCommunication = new Communication(this);
    mMainWindow = new MainWindow();

    mWorkingTimer.setInterval(250);
    connect(&mWorkingTimer, &QTimer::timeout, this, &Application::slotWorkingCycle);

    QTimer::singleShot(0, this, SLOT(slotAppRun()));
}

Application::~Application() {
}

void Application::slotAppRun() {
    connect(mMainWindow, &MainWindow::onConnectionChanged, mCommunication, &Communication::openSerialPort);
    connect(mCommunication, &Communication::onSerialPortOpened, mMainWindow, &MainWindow::slotSerialPortOpened);
    connect(mCommunication, &Communication::onSerialPortClosed, mMainWindow, &MainWindow::slotSerialPortClosed);
    connect(mCommunication, &Communication::onSerialPortOpened, this, &Application::slotSerialPortOpened);
    connect(mCommunication, &Communication::onSerialPortClosed, this, &Application::slotSerialPortClosed);

    connect(mCommunication, &Communication::onDeviceInfo, mMainWindow, &MainWindow::slotDisplayDeviceInfo);

    connect(mCommunication, &Communication::onActiveSetting, mMainWindow, &MainWindow::slotEnableMemoryKey);
    connect(mMainWindow, &MainWindow::onMemoryKey, mCommunication, &Communication::RecallSetting);


    connect(mCommunication, &Communication::onSetCurrent, mMainWindow, &MainWindow::slotDisplayCurrent);
    connect(mCommunication, &Communication::onOutputCurrent, mMainWindow, &MainWindow::slotDisplayCurrent);
    connect(mCommunication, &Communication::onSetVoltage, mMainWindow, &MainWindow::slotDisplayVoltage);
    connect(mCommunication, &Communication::onOutputVoltage, mMainWindow, &MainWindow::slotDisplayVoltage);

    connect(mCommunication, &Communication::onOutputStatus, mMainWindow, &MainWindow::slotUpdateOutputStatus);

    //
    connect(mMainWindow, &MainWindow::onVoltageChanged, this, &Application::slotVoltageChanged);
    connect(mMainWindow, &MainWindow::onCurrentChanged, this, &Application::slotCurrentChanged);


//    QTimer::singleShot(1000, this, [=]() {
//        mMainWindow->slotUpdateOutputStatus(ConstantCurrent, ConstantVoltage, false);
//    });
//    QTimer::singleShot(2000, this, [=]() {
//        mMainWindow->slotUpdateOutputStatus(ConstantVoltage, ConstantVoltage, true);
//    });
//    QTimer::singleShot(3000, this, [=]() {
//        mMainWindow->slotUpdateOutputStatus(ConstantVoltage, ConstantCurrent, false);
//    });

    mMainWindow->show();
}

void Application::slotSerialPortOpened() {
    mCommunication->ReadDeviceInfo();

    mWorkingTimer.start();
}

void Application::slotSerialPortClosed() {
    mWorkingTimer.stop();
}


void Application::slotWorkingCycle() {
    mCommunication->ReadOutputStatus();
    mCommunication->ReadActiveSetting();

    mCommunication->ReadSetCurrent(Channel1);
    mCommunication->ReadSetCurrent(Channel2);
    mCommunication->ReadSetVoltage(Channel1);
    mCommunication->ReadSetVoltage(Channel2);
}




void Application::slotVoltageChanged(TChannel channel, double value) {
    qDebug() << "slotVoltageChanged" << channel << value;
}

void Application::slotCurrentChanged(TChannel channel, double value) {
 //   qDebug() << "slotCurrentChanged" << channel << value;
    mCommunication->SetCurrent(channel, value);
    mCommunication->ReadSetCurrent(channel);
}






