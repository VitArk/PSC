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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "Global.h"
#include "Settings.h"
#include "CommunicationMetrics.h"
#include "widgets/ClickableLabel.h"
#include "widgets/DialWidget.h"
#include "widgets/ProtectionWidget.h"
#include "widgets/DisplayWidget.h"
#include "widgets/PresetWidget.h"
#include "widgets/ProtectionControlWidget.h"
#include "widgets/ChannelsTrackingWidget.h"
#include "widgets/OutputSwitch.h"
#include "widgets/StatusBar.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void autoOpenSerialPort();

signals:
    void onSerialPortSettingsChanged(QString serialPortName, int baudRate);
    void onSerialPortDoClose();
    void onSetChannelsTracking(Global::ChannelsTracking tracking);
    void onSetEnableOutputProtection(Global::OutputProtection protection);
    void onSetOverVoltageProtectionValue(Global::Channel channel, double value);
    void onSetOverCurrentProtectionValue(Global::Channel channel, double value);
    void onSetPreset(Global::MemoryKey key);
    void onSavePreset(Global::MemoryKey key);
    void onSetVoltage(Global::Channel channel, double value);
    void onSetCurrent(Global::Channel channel, double value);
    void onSetEnableOutputSwitch(bool state);
    void onSetLocked(bool enable);
    void onSetEnabledBeep(bool enable);

public slots:
    void SerialPortOpened(const QString &serialPortName, int baudRate);
    void SerialPortClosed();
    void SerialPortErrorOccurred(const QString &error);
    void ConnectionDeviceReady(const Global::DeviceInfo &info);
    void ConnectionUnknownDevice(const QString &deviceID);
    void UpdateCommunicationMetrics(const CommunicationMetrics &info);
    void UpdateChannelTrackingMode(Global::ChannelsTracking tracking);
    void UpdateOutputProtectionMode(Global::OutputProtection protection);
    void UpdateChannelMode(Global::Channel channel, Global::OutputMode mode);
    void UpdateActivePreset(Global::MemoryKey key);
    void UpdateActualVoltage(Global::Channel channel, double voltage);
    void UpdateActualCurrent(Global::Channel channel, double current);
    void UpdateVoltageSet(Global::Channel channel, double voltage);
    void UpdateCurrentSet(Global::Channel channel, double current);
    void UpdateOverVoltageProtectionSet(Global::Channel channel, double voltage);
    void UpdateOverCurrentProtectionSet(Global::Channel channel, double current);
    void SetEnableOutputSwitch(bool enable);
    void SetEnableLock(bool enable);
    void SetEnableBeep(bool enable);

private slots:
    void SerialPortChanged(bool toggled);
    void SetEnableReadonlyMode(bool enable);
    void CreateSerialPortMenuItems();
    static void ShowAboutBox();
    void ShowDeviceNameOrID();

private:
    void setupUI();
    template<typename func> DialWidget *createDialWidget(const QString &title, const QString &suffix, double precision, func lambdaFn);
    ProtectionWidget *createProtectionWidget(Global::Channel ch);
    bool acceptEnable() const;
    void enableControls(bool enable);
    void setControlLimits(const Global::DeviceInfo &info);
    void enableChannel(Global::Channel ch, bool enable);
    void createBaudRatesMenu();
    QString chosenSerialPort() const;
    int chosenBaudRates(int defaultValue = 9600) const;

private:
    Ui::MainWindow *ui;
    Settings mSettings;

    QMap<Global::Channel, DialWidget*>          mInputVoltage;
    QMap<Global::Channel, DialWidget*>          mInputCurrent;
    QMap<Global::Channel, ProtectionWidget*>    mProtectionSet;
    QMap<Global::Channel, DisplayWidget*>       mDisplay;
    ProtectionControlWidget                     *mProtectionControl = nullptr;
    PresetWidget                                *mPreset = nullptr;
    OutputSwitch                                *mOutputSwitch = nullptr;
    ChannelsTrackingWidget                      *mChannelsTracking = nullptr;
    StatusBar                                   *mStatusBar = nullptr;

    bool mIsSerialConnected = false;
    Global::DeviceInfo mDeviceInfo;
};

#endif // MAINWINDOW_H
