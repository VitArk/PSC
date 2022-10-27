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

    mDeviceUpdaterTimer.setTimerType(Qt::PreciseTimer);
    mDeviceUpdaterTimer.setInterval(WORKING_TIMER_INTERVAL_MIN); // 150 min (9600), 250 norm.
    connect(&mDeviceUpdaterTimer, &QTimer::timeout, this, &Application::DeviceUpdateCycle);

    QTimer::singleShot(0, this, SLOT(Run()));
}

Application::~Application() {
}

void Application::Run() {
    connect(mMainWindow, &MainWindow::onSerialPortSettingsChanged, mCommunication, &Communication::OpenSerialPort);
    connect(mMainWindow, &MainWindow::onSerialPortDoClose, mCommunication, &Communication::CloseSerialPort);
    connect(mCommunication, &Communication::onMetricsReady, mMainWindow, &MainWindow::UpdateCommunicationMetrics);
    connect(mCommunication, &Communication::onMetricsReady, this, &Application::TuneDeviceUpdaterTimerInterval);

    connect(mCommunication, &Communication::onSerialPortErrorOccurred, mMainWindow, &MainWindow::SerialPortErrorOccurred);
    connect(mCommunication, &Communication::onSerialPortOpened, mMainWindow, &MainWindow::SerialPortOpened);
    connect(mCommunication, &Communication::onSerialPortClosed, this, &Application::SerialPortClosed);
    connect(mCommunication, &Communication::onDeviceReady, this, &Application::DeviceReady);
    connect(mCommunication, &Communication::onUnknownDevice, mMainWindow, &MainWindow::ConnectionUnknownDevice);

    // Lock operation panel
    connect(mMainWindow, &MainWindow::onSetLocked, mCommunication, &Communication::SetLocked);
    connect(mCommunication, &Communication::onGetIsLocked, mMainWindow, &MainWindow::SetEnableLock);

    // Buzzer
    connect(mMainWindow, &MainWindow::onSetEnabledBeep, mCommunication, &Communication::SetEnableBeep);
    connect(mCommunication, &Communication::onGetIsBeepEnabled, mMainWindow, &MainWindow::SetEnableBeep);

    // Memory / Presets
    connect(mCommunication, &Communication::onGetPreset, mMainWindow, &MainWindow::UpdateActivePreset);
    connect(mMainWindow, &MainWindow::onSetPreset, mCommunication, &Communication::SetPreset);
    connect(mMainWindow, &MainWindow::onSavePreset, mCommunication, &Communication::SavePreset);

    connect(mMainWindow, &MainWindow::onSetEnableOutputSwitch, mCommunication, &Communication::SetEnableOutputSwitch);
    connect(mMainWindow, &MainWindow::onSetChannelTracking, mCommunication,
            &Communication::SetChannelTracking);
    connect(mMainWindow, &MainWindow::onSetEnableOutputProtection, this, &Application::OutputProtectionChanged);
    connect(mCommunication, &Communication::onGetOverCurrentProtectionValue, mMainWindow,
            &MainWindow::UpdateOverCurrentProtectionSet);
    connect(mCommunication, &Communication::onGetOverVoltageProtectionValue, mMainWindow,
            &MainWindow::UpdateOverVoltageProtectionSet);
    connect(mMainWindow, &MainWindow::onSetOverCurrentProtectionValue, mCommunication,
            &Communication::SetOverCurrentProtectionValue);
    connect(mMainWindow, &MainWindow::onSetOverVoltageProtectionValue, mCommunication,
            &Communication::SetOverVoltageProtectionValue);

    connect(mCommunication, &Communication::onGetCurrentSet, mMainWindow, &MainWindow::UpdateCurrentSet);
    connect(mCommunication, &Communication::onGetActualCurrent, mMainWindow, &MainWindow::UpdateActualCurrent);

    connect(mCommunication, &Communication::onGetVoltageSet, mMainWindow, &MainWindow::UpdateVoltageSet);
    connect(mCommunication, &Communication::onGetActualVoltage, mMainWindow, &MainWindow::UpdateActualVoltage);

    connect(mCommunication, &Communication::onGetDeviceStatus, this, &Application::OutputStatus);

    connect(mMainWindow, &MainWindow::onSetVoltage, mCommunication, &Communication::SetVoltage);
    connect(mMainWindow, &MainWindow::onSetCurrent, mCommunication, &Communication::SetCurrent);

    mMainWindow->show();
    mMainWindow->autoOpenSerialPort();
}

void Application::DeviceReady(const Protocol::DeviceInfo &info) {
    mMainWindow->ConnectionDeviceReady(info);

//    mCommunication->GetDeviceID();
    mCommunication->GetOverCurrentProtectionValue(Protocol::Channel1);
    mCommunication->GetOverCurrentProtectionValue(Protocol::Channel2);
    mCommunication->GetOverVoltageProtectionValue(Protocol::Channel1);
    mCommunication->GetOverVoltageProtectionValue(Protocol::Channel2);

    mDeviceUpdaterTimer.start();
}

void Application::SerialPortClosed() {
    mDeviceUpdaterTimer.stop();
    mMainWindow->SerialPortClosed();
}

void Application::DeviceUpdateCycle() {
    mCommunication->GetDeviceStatus();
    mCommunication->GetPreset();
    mCommunication->GetIsLocked();
    mCommunication->GetIsBuzzerEnabled();
}

void Application::OutputStatus(const Protocol::DeviceStatus &status) {
    if (status.OutputSwitch) {
        mCommunication->GetActualCurrent(Protocol::Channel1);
        mCommunication->GetActualCurrent(Protocol::Channel2);
        mCommunication->GetActualVoltage(Protocol::Channel1);
        mCommunication->GetActualVoltage(Protocol::Channel2);

        disconnect(mCommunication, &Communication::onGetCurrentSet, mMainWindow, &MainWindow::UpdateActualCurrent);
        disconnect(mCommunication, &Communication::onGetVoltageSet, mMainWindow, &MainWindow::UpdateActualVoltage);
    } else {
        connect(mCommunication, &Communication::onGetCurrentSet, mMainWindow, &MainWindow::UpdateActualCurrent);
        connect(mCommunication, &Communication::onGetVoltageSet, mMainWindow, &MainWindow::UpdateActualVoltage);
    }

    mCommunication->GetCurrentSet(Protocol::Channel1);
    mCommunication->GetCurrentSet(Protocol::Channel2);
    mCommunication->GetVoltageSet(Protocol::Channel1);
    mCommunication->GetVoltageSet(Protocol::Channel2);

    if (status.Protection == Protocol::OverVoltageProtectionOnly || status.Protection == Protocol::OutputProtectionAllEnabled) {
        mCommunication->GetOverVoltageProtectionValue(Protocol::Channel1);
        mCommunication->GetOverVoltageProtectionValue(Protocol::Channel2);
    }
    if (status.Protection == Protocol::OverCurrentProtectionOnly || status.Protection == Protocol::OutputProtectionAllEnabled) {
        mCommunication->GetOverCurrentProtectionValue(Protocol::Channel1);
        mCommunication->GetOverCurrentProtectionValue(Protocol::Channel2);
    }

    mMainWindow->UpdateChannelMode(Protocol::Channel1, status.ModeCh1);
    mMainWindow->UpdateChannelMode(Protocol::Channel2, status.ModeCh2);
    mMainWindow->SetEnableOutputSwitch(status.OutputSwitch);
    mMainWindow->UpdateOutputProtectionMode(status.Protection);
    mMainWindow->UpdateChannelTrackingMode(status.Tracking);
}

void Application::OutputProtectionChanged(Protocol::OutputProtection protection) {
    mCommunication->SetEnableOverVoltageProtection(
            protection == Protocol::OverVoltageProtectionOnly || protection == Protocol::OutputProtectionAllEnabled);
    mCommunication->SetEnableOverCurrentProtection(
            protection == Protocol::OverCurrentProtectionOnly || protection == Protocol::OutputProtectionAllEnabled);
}

void Application::TuneDeviceUpdaterTimerInterval(const CommunicationMetrics &metrics) {
    int interval = mDeviceUpdaterTimer.interval();
    if (metrics.queueLength() > 2) {
        interval = qMin(interval + WORKING_TIMER_INTERVAL_STEP, WORKING_TIMER_INTERVAL_MAX);
    } else {
        interval = qMax(interval - WORKING_TIMER_INTERVAL_STEP, WORKING_TIMER_INTERVAL_MIN);
    }
    mDeviceUpdaterTimer.setInterval(interval);
}








