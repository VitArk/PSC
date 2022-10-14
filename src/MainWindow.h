#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "devices/Commons.h"
#include "Debounce.h"
#include "Settings.h"

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

    void onOutputConnectionMethodChanged(TOutputConnectionMethod method);
    void onOutputProtectionChanged(TOutputProtection protection);
    void onOverVoltageProtectionChanged(TChannel channel, double value);
    void onOverCurrentProtectionChanged(TChannel channel, double value);
    void onMemoryKeyChanged(TMemoryKey key);
    void onVoltageChanged(TChannel channel, double value);
    void onCurrentChanged(TChannel channel, double value);
    void onOutputSwitchChanged(bool state);
    void onLockOperationPanelChanged(bool lock);
    void onBuzzerChanged(bool lock);

public slots:
    void slotSerialPortOpened(const QString& serialPortName, int baudRate);
    void slotSerialPortClosed();
    void slotSerialPortErrorOccurred(QString error);
    void slotSerialConnectionReady(DeviceInfo info);

    void slotShowOutputConnectionMethod(OutputConnectionMethod method);
    void slotShowOutputProtectionMode(OutputProtection protection);
    void slotShowOutputStabilizingMode(TOutputStabilizingMode channel1, TOutputStabilizingMode channel2);
    void slotShowOutputSwitchStatus(bool state);
    void slotEnableMemoryKey(TMemoryKey key);

    void slotDisplayOutputVoltage(TChannel channel, double voltage);
    void slotDisplayOutputCurrent(TChannel channel, double current);
    void slotDisplaySetVoltage(TChannel channel, double voltage);
    void slotDisplaySetCurrent(TChannel channel, double current);

    void slotDisplayOverVoltageProtectionValue(TChannel channel, double voltage);
    void slotDisplayOverCurrentProtectionValue(TChannel channel, double current);

    void slotDisplayDeviceID(const QString &deviceID);
    void slotLockOperationPanel(bool lock);
    void slotEnableBuzzer(bool enabled);

private slots:
    void slotOutputConnectionMethodChanged();
    void slotOutputProtectionChanged();

    void slotMemoryKeyChanged(bool toggle);

    void slotDialControlChanged();
    void slotSpinControlChanged();
    void slotControlValueChanged();
    void slotControlValueChangedDebounced(double value);
    void slotOverProtectionChanged();
    void slotEnableReadonlyMode(bool enable);

    // Serial port
    void slotSerialPortConnectionToggled(bool toggled);
    void slotCreateSerialPortMenuItems();

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
    QLabel *mStatusBarDeviceInfo;
    QLabel *mStatusBarDeviceLock;

    bool mIsSerialConnected = false;

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
