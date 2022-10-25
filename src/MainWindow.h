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

    void onOutputConnectionMethodChanged(TChannelTracking method);
    void onOutputProtectionChanged(TOutputProtection protection);
    void onOverVoltageProtectionChanged(TChannel channel, double value);
    void onOverCurrentProtectionChanged(TChannel channel, double value);
    void onPresetChanged(TMemoryKey key);
    void onPresetSave(TMemoryKey key);
    void onVoltageChanged(TChannel channel, double value);
    void onCurrentChanged(TChannel channel, double value);
    void onOutputSwitchChanged(bool state);
    void onLockOperationPanelChanged(bool lock);
    void onBuzzerChanged(bool lock);

public slots:
    void slotSerialPortOpened(const QString& serialPortName, int baudRate);
    void slotSerialPortClosed();
    void slotSerialPortErrorOccurred(QString error);
    void slotDeviceReady(DeviceInfo info);
    void slotUnknownDevice(QString deviceID);
    void slotShowCommunicationMetrics(const CommunicationMetrics &info);

    void slotShowOutputTrackingMode(TChannelTracking method);
    void slotShowOutputProtectionMode(TOutputProtection protection);
    void slotShowOutputMode(TOutputMode channel1, TOutputMode channel2);
    void slotShowOutputSwitchStatus(bool state);
    void slotEnableMemoryKey(TMemoryKey key);

    void slotDisplayOutputVoltage(TChannel channel, double voltage);
    void slotDisplayOutputCurrent(TChannel channel, double current);
    void slotDisplaySetVoltage(TChannel channel, double voltage);
    void slotDisplaySetCurrent(TChannel channel, double current);

    void slotDisplayOverVoltageProtectionValue(TChannel channel, double voltage);
    void slotDisplayOverCurrentProtectionValue(TChannel channel, double current);

    void slotLockOperationPanel(bool lock);
    void slotEnableBuzzer(bool enabled);

private slots:
    void slotOutputConnectionMethodChanged();
    void slotOutputProtectionChanged();
    void slotPresetKeyClicked();
    void slotPresetSaveClicked();

    void slotDialControlChanged();
    void slotSpinControlChanged();
    void slotControlValueChanged();
    void slotControlValueChangedDebounced(double value);
    void slotOverProtectionChanged();
    void slotEnableReadonlyMode(bool enable);

    // Serial port
    void slotSerialPortConnectionToggled(bool toggled);
    void slotCreateSerialPortMenuItems();

    void slotShowAboutBox();

    void slotShowDeviceNameOrID();

private:
    enum {
        HighlightNone = 0,
        HighlightRed = 1,
        HighlightGreen = 2,
    } typedef THighlight;

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
    DeviceInfo  mDeviceInfo;

private:
    bool acceptEnable() const;
    void enableControls(bool enable);
    void setControlLimits(const DeviceInfo &info);
    void enableChannel(TChannel ch, bool enable);
    void openSvg(const QString &resource);
    void createBaudRatesMenu();
    QString chosenSerialPort() const;
    int chosenBaudRates(int defaultValue = 9600) const;

    static void highlight(THighlight color, QLabel *label);
    static QString currentFormat(double value);
    static QString voltageFormat(double value);
};

#endif // MAINWINDOW_H
