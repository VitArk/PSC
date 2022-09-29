#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "Commons.h"
#include "Debounce.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void onOutputConnectionMethodChanged(TOutputConnectionMethod method);
    void onOutputProtectionChanged(TOutputProtection protection);
    void onOverVoltageProtectionChanged(TChannel channel, double value);
    void onOverCurrentProtectionChanged(TChannel channel, double value);
    void onMemoryKeyChanged(TMemoryKey key);
    void onVoltageChanged(TChannel channel, double value);
    void onCurrentChanged(TChannel channel, double value);
    void onOutputSwitchChanged(bool state);

    void onSerialPortSettingsChanged(QString serialPortName, int baudRate);
    void onLockOperationPanelChanged(bool lock);
    void onBuzzerChanged(bool lock);

public slots:
    void slotSerialPortOpened();
    void slotSerialPortClosed();
    void slotSerialPortErrorOccurred(QString error);

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

    void slotDisplayDeviceInfo(const QString &deviceInfo);
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

private:
    enum {
        HighlightNone = 0,
        HighlightRed = 1,
        HighlightGreen = 2,
    } typedef THighlight;

    Ui::MainWindow *ui;
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
    void enableChannel(TChannel ch, bool enable);

    void openSvg(const QString &resource);

    void createSerialPortMenu();
    void createBaudRatesMenu(int defaultValue = 9600);
    QString chosenSerialPort() const;
    int chosenBaudRates(int defaultValue = 9600) const;

    static void highlight(THighlight color, QLabel *label);
    static QString currentFormat(double value) ;
    static QString voltageFormat(double value) ;
};

#endif // MAINWINDOW_H
