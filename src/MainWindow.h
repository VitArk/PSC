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
#include <QLabel>
#include "Global.h"
#include "Settings.h"
#include "CommunicationMetrics.h"
#include "ClickableLabel.h"
#include "DialWidget.h"
#include "ProtectionWidget.h"
#include "DisplayWidget.h"
#include "PresetWidget.h"
#include "ProtectionControlWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void autoOpenSerialPort();

signals:
    void onSerialPortSettingsChanged(QString serialPortName, int baudRate);
    void onSerialPortDoClose();

    void onSetChannelTracking(Global::ChannelTracking tracking);
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
    void SerialPortOpened(const QString& serialPortName, int baudRate);
    void SerialPortClosed();
    void SerialPortErrorOccurred(const QString &error);

    void ConnectionDeviceReady(const Global::DeviceInfo &info);
    void ConnectionUnknownDevice(QString deviceID);

    void UpdateCommunicationMetrics(const CommunicationMetrics &info);

    void UpdateChannelTrackingMode(Global::ChannelTracking tracking);
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
    // Serial port
    void SerialPortChanged(bool toggled);

    void ChannelTrackingChanged();
    void SetEnableReadonlyMode(bool enable);

    void slotCreateSerialPortMenuItems(); // TODO ????

    void ShowAboutBox();
    void ShowDeviceNameOrID();

private:
    Ui::MainWindow *ui;
    Settings mSettings;

    QLabel *mStatusBarConnectionStatus;
    ClickableLabel *mStatusBarDeviceInfo;
    QLabel *mStatusBarDeviceLock;
    QLabel *mStatusCommunicationMetrics;

    DialWidget   *mInputCh1V;
    DialWidget   *mInputCh1A;
    DialWidget   *mInputCh2V;
    DialWidget   *mInputCh2A;

    ProtectionWidget *mProtectionSetCh1;
    ProtectionWidget *mProtectionSetCh2;
    ProtectionControlWidget *mProtectionControl;

    DisplayWidget *mDisplayCh1;
    DisplayWidget *mDisplayCh2;

    PresetWidget  *mPreset;

    bool mIsSerialConnected = false;
    Global::DeviceInfo  mDeviceInfo;

private:
    bool acceptEnable() const;
    void enableControls(bool enable);
    void setControlLimits(const Global::DeviceInfo &info);
    void enableChannel(Global::Channel ch, bool enable);
    void openSvg(const QString &resource);
    void createBaudRatesMenu();
    QString chosenSerialPort() const;
    int chosenBaudRates(int defaultValue = 9600) const;
};

#endif // MAINWINDOW_H
