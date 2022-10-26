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

#include "Application.h"
#include <QTimer>
#include <QDebug>

#define WORKING_TIMER_INTERVAL_MIN 250
#define WORKING_TIMER_INTERVAL_MAX 500
#define WORKING_TIMER_INTERVAL_STEP 25

Application::Application(int &argc, char **argv, int) : QApplication(argc, argv) {
    mCommunication = new Communication(this);
    mMainWindow = new MainWindow();

    mWorkingTimer.setTimerType(Qt::PreciseTimer);
    mWorkingTimer.setInterval(WORKING_TIMER_INTERVAL_MIN); // 150 min (9600), 250 norm.
    connect(&mWorkingTimer, &QTimer::timeout, this, &Application::slotWorkingCycle);

    QTimer::singleShot(0, this, SLOT(slotAppRun()));
}

Application::~Application() {
}

void Application::slotAppRun() {
    connect(mMainWindow, &MainWindow::onSerialPortSettingsChanged, mCommunication, &Communication::openSerialPort);
    connect(mMainWindow, &MainWindow::onSerialPortDoClose, mCommunication, &Communication::closeSerialPort);
    connect(mCommunication, &Communication::onMetricsReady, mMainWindow, &MainWindow::slotShowCommunicationMetrics);
    connect(mCommunication, &Communication::onMetricsReady, this, &Application::slotTuneWorkingTimerInterval);

    connect(mCommunication, &Communication::onSerialPortErrorOccurred, mMainWindow, &MainWindow::slotSerialPortErrorOccurred);
    connect(mCommunication, &Communication::onSerialPortOpened, mMainWindow, &MainWindow::slotSerialPortOpened);
    connect(mCommunication, &Communication::onSerialPortClosed, this, &Application::slotSerialPortClosed);
    connect(mCommunication, &Communication::onDeviceReady, this, &Application::slotStartWorking);
    connect(mCommunication, &Communication::onUnknownDevice, mMainWindow, &MainWindow::slotUnknownDevice);

    // Lock operation panel
    connect(mMainWindow, &MainWindow::onLockOperationPanelChanged, mCommunication, &Communication::setLocked);
    connect(mCommunication, &Communication::onGetIsLocked, mMainWindow, &MainWindow::slotLockOperationPanel);

    // Buzzer
    connect(mMainWindow, &MainWindow::onBuzzerChanged, mCommunication, &Communication::setEnableBuzzer);
    connect(mCommunication, &Communication::onGetIsBeepEnabled, mMainWindow, &MainWindow::slotEnableBuzzer);

    // Memory / Presets
    connect(mCommunication, &Communication::onGetPreset, mMainWindow, &MainWindow::slotEnableMemoryKey);
    connect(mMainWindow, &MainWindow::onPresetChanged, mCommunication, &Communication::setPreset);
    connect(mMainWindow, &MainWindow::onPresetSave, mCommunication, &Communication::savePreset);

    connect(mMainWindow, &MainWindow::onOutputSwitchChanged, mCommunication, &Communication::setEnableOutputSwitch);
    connect(mMainWindow, &MainWindow::onOutputConnectionMethodChanged, mCommunication,
            &Communication::setChannelTracking);
    connect(mMainWindow, &MainWindow::onOutputProtectionChanged, this, &Application::slotOutputProtectionChanged);
    connect(mCommunication, &Communication::onGetOverCurrentProtectionValue, mMainWindow, &MainWindow::slotDisplayOverCurrentProtectionValue);
    connect(mCommunication, &Communication::onGetOverVoltageProtectionValue, mMainWindow, &MainWindow::slotDisplayOverVoltageProtectionValue);
    connect(mMainWindow, &MainWindow::onOverCurrentProtectionChanged, mCommunication, &Communication::setOverCurrentProtectionValue);
    connect(mMainWindow, &MainWindow::onOverVoltageProtectionChanged, mCommunication, &Communication::setOverVoltageProtectionValue);

    connect(mCommunication, &Communication::onGetCurrentSet, mMainWindow, &MainWindow::slotDisplaySetCurrent);
    connect(mCommunication, &Communication::onGetActualCurrent, mMainWindow, &MainWindow::slotDisplayOutputCurrent);

    connect(mCommunication, &Communication::onGetVoltageSet, mMainWindow, &MainWindow::slotDisplaySetVoltage);
    connect(mCommunication, &Communication::onGetActualVoltage, mMainWindow, &MainWindow::slotDisplayOutputVoltage);

    connect(mCommunication, &Communication::onGetDeviceStatus, this, &Application::slotOutputStatus);

    connect(mMainWindow, &MainWindow::onVoltageChanged, mCommunication, &Communication::setVoltage);
    connect(mMainWindow, &MainWindow::onCurrentChanged, mCommunication, &Communication::setCurrent);

    mMainWindow->show();
    mMainWindow->autoOpenSerialPort();
}

void Application::slotStartWorking(const Protocol::DeviceInfo &info) {
    mMainWindow->slotDeviceReady(info);

//    mCommunication->getDeviceID();
    mCommunication->getOverCurrentProtectionValue(Protocol::Channel1);
    mCommunication->getOverCurrentProtectionValue(Protocol::Channel2);
    mCommunication->getOverVoltageProtectionValue(Protocol::Channel1);
    mCommunication->getOverVoltageProtectionValue(Protocol::Channel2);

    mWorkingTimer.start();
}

void Application::slotSerialPortClosed() {
    mWorkingTimer.stop();
    mMainWindow->slotSerialPortClosed();
}

void Application::slotWorkingCycle() {
    mCommunication->getDeviceStatus();
    mCommunication->getPreset();
    mCommunication->getIsLocked();
    mCommunication->getIsBuzzerEnabled();
}

void Application::slotOutputStatus(const Protocol::DeviceStatus &status) {
    if (status.outputSwitch) {
        mCommunication->getActualCurrent(Protocol::Channel1);
        mCommunication->getActualCurrent(Protocol::Channel2);
        mCommunication->getActualVoltage(Protocol::Channel1);
        mCommunication->getActualVoltage(Protocol::Channel2);

        disconnect(mCommunication, &Communication::onGetCurrentSet, mMainWindow, &MainWindow::slotDisplayOutputCurrent);
        disconnect(mCommunication, &Communication::onGetVoltageSet, mMainWindow, &MainWindow::slotDisplayOutputVoltage);
    } else {
        connect(mCommunication, &Communication::onGetCurrentSet, mMainWindow, &MainWindow::slotDisplayOutputCurrent);
        connect(mCommunication, &Communication::onGetVoltageSet, mMainWindow, &MainWindow::slotDisplayOutputVoltage);
    }

    mCommunication->getCurrentSet(Protocol::Channel1);
    mCommunication->getCurrentSet(Protocol::Channel2);
    mCommunication->getVoltageSet(Protocol::Channel1);
    mCommunication->getVoltageSet(Protocol::Channel2);

    if (status.protection == Protocol::OverVoltageProtectionOnly || status.protection == Protocol::OutputProtectionAllEnabled) {
        mCommunication->getOverVoltageProtectionValue(Protocol::Channel1);
        mCommunication->getOverVoltageProtectionValue(Protocol::Channel2);
    }
    if (status.protection == Protocol::OverCurrentProtectionOnly || status.protection == Protocol::OutputProtectionAllEnabled) {
        mCommunication->getOverCurrentProtectionValue(Protocol::Channel1);
        mCommunication->getOverCurrentProtectionValue(Protocol::Channel2);
    }

    mMainWindow->slotShowOutputMode(status.channel1Mode, status.channel2Mode);
    mMainWindow->slotShowOutputSwitchStatus(status.outputSwitch);
    mMainWindow->slotShowOutputProtectionMode(status.protection);
    mMainWindow->slotShowOutputTrackingMode(status.tracking);
}

void Application::slotOutputProtectionChanged(Protocol::OutputProtection protection) {
    mCommunication->setEnableOverVoltageProtection(
            protection == Protocol::OverVoltageProtectionOnly || protection == Protocol::OutputProtectionAllEnabled);
    mCommunication->setEnableOverCurrentProtection(
            protection == Protocol::OverCurrentProtectionOnly || protection == Protocol::OutputProtectionAllEnabled);
}

void Application::slotTuneWorkingTimerInterval(const CommunicationMetrics &metrics) {
    int interval = mWorkingTimer.interval();
    if (metrics.queueLength() > 2) {
        interval = qMin(interval + WORKING_TIMER_INTERVAL_STEP, WORKING_TIMER_INTERVAL_MAX);
    } else {
        interval = qMax(interval - WORKING_TIMER_INTERVAL_STEP, WORKING_TIMER_INTERVAL_MIN);
    }
    mWorkingTimer.setInterval(interval);
}








