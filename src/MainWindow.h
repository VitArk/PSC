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
#include "protocol/Commons.h"
#include "Debounce.h"
#include "Settings.h"
#include "CommunicationMetrics.h"
#include "ClickableLabel.h"

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

    void onSetChannelTracking(Protocol::ChannelTracking tracking);
    void onSetEnableOutputProtection(Protocol::OutputProtection protection);
    void onSetOverVoltageProtectionValue(Protocol::Channel channel, double value);
    void onSetOverCurrentProtectionValue(Protocol::Channel channel, double value);
    void onSetPreset(Protocol::MemoryKey key);
    void onSavePreset(Protocol::MemoryKey key);
    void onSetVoltage(Protocol::Channel channel, double value);
    void onSetCurrent(Protocol::Channel channel, double value);
    void onSetEnableOutputSwitch(bool state);
    void onSetLocked(bool enable);
    void onSetEnabledBeep(bool enable);

public slots:
    void SerialPortOpened(const QString& serialPortName, int baudRate);
    void SerialPortClosed();
    void SerialPortErrorOccurred(QString error);

    void ConnectionDeviceReady(const Protocol::DeviceInfo &info);
    void ConnectionUnknownDevice(QString deviceID);

    void UpdateCommunicationMetrics(const CommunicationMetrics &info);

    void UpdateChannelTrackingMode(Protocol::ChannelTracking tracking);
    void UpdateOutputProtectionMode(Protocol::OutputProtection protection);
    void UpdateChannelMode(Protocol::Channel channel, Protocol::OutputMode mode);
    void UpdateActivePreset(Protocol::MemoryKey key);
    void UpdateActualVoltage(Protocol::Channel channel, double voltage);
    void UpdateActualCurrent(Protocol::Channel channel, double current);
    void UpdateVoltageSet(Protocol::Channel channel, double voltage);
    void UpdateCurrentSet(Protocol::Channel channel, double current);
    void UpdateOverVoltageProtectionSet(Protocol::Channel channel, double voltage);
    void UpdateOverCurrentProtectionSet(Protocol::Channel channel, double current);
    void SetEnableOutputSwitch(bool enable);
    void SetEnableLock(bool enable);
    void SetEnableBeep(bool enable);

private slots:
    // Serial port
    void SerialPortChanged(bool toggled);

    void ChannelTrackingChanged();
    void OutputProtectionChanged(); // TODO ???? what do the method comparing to OverProtectionValueChanged
    void PresetChanged();
    void PresetSave();
    void SetEnableReadonlyMode(bool enable);


    void slotDialControlChanged();
    void slotSpinControlChanged();
    void slotControlValueChanged();
    void slotControlValueChangedDebounced(double value);

    void OverProtectionValueChanged();

    void slotCreateSerialPortMenuItems(); // TODO ????

    void ShowAboutBox();
    void ShowDeviceNameOrID();

private:
    Ui::MainWindow *ui;
    Settings mSettings;

    Debounce mDebouncedCh1V;
    Debounce mDebouncedCh1A;
    Debounce mDebouncedCh2V;
    Debounce mDebouncedCh2A;

    QLabel *mStatusBarConnectionStatus;
    ClickableLabel *mStatusBarDeviceInfo;
    QLabel *mStatusBarDeviceLock;
    QLabel *mStatusCommunicationMetrics;

    bool mIsSerialConnected = false;
    Protocol::DeviceInfo  mDeviceInfo;

private:
    bool acceptEnable() const;
    void enableControls(bool enable);
    void setControlLimits(const Protocol::DeviceInfo &info);
    void enableChannel(Protocol::Channel ch, bool enable);
    void openSvg(const QString &resource);
    void createBaudRatesMenu();
    QString chosenSerialPort() const;
    int chosenBaudRates(int defaultValue = 9600) const;

    static QString currentFormat(double value);
    static QString voltageFormat(double value);
};

#endif // MAINWINDOW_H
