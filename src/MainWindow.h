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

    void showOutputConnectionMethod(OutputConnectionMethod method);
    void showOutputProtectionMode(OutputProtection protection);
    void showOutputStabilizingMode(TOutputStabilizingMode channel1, TOutputStabilizingMode channel2);
    void showOutputSwitchStatus(bool state);

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

public slots:
    void slotSerialPortOpened();
    void slotSerialPortClosed();

    void slotEnableMemoryKey(TMemoryKey key);
    void slotDisplayOutputVoltage(TChannel channel, double voltage);
    void slotDisplayOutputCurrent(TChannel channel, double current);
    void slotDisplaySetVoltage(TChannel channel, double voltage);
    void slotDisplaySetCurrent(TChannel channel, double current);

    void slotDisplayOverVoltageProtectionValue(TChannel channel, double voltage);
    void slotDisplayOverCurrentProtectionValue(TChannel channel, double current);

    void slotDisplayDeviceInfo(const QString &deviceInfo);

private slots:
    void slotOutputConnectionMethodChanged();
    void slotOutputProtectionChanged();

    void slotMemoryKeyChanged(bool toggle);

    void slotDialControlChanged();
    void slotSpinControlChanged();
    void slotControlValueChanged();
    void slotControlValueChangedDebounced(double value);
    void slotOverProtectionChanged();

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

private:
    void enableChannel(TChannel ch, bool enable);
    void enableOperationPanel(bool enable);

    void openSvg(const QString &resource);

    void highlight(THighlight color, QLabel *label);

    void createSerialPortMenu();
    void createBaudRatesMenu(int defaultValue = 9600);
    QString chosenSerialPort() const;
    int chosenBaudRates(int defaultValue = 9600) const;

    void resetStatusBarText();

    static QString currentFormat(double value) ;
    static QString voltageFormat(double value) ;
};

#endif // MAINWINDOW_H
