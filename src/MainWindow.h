#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "Commons.h"
#include "ControlDebounce.h"

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
    void onIndependentMode();
    void onSerialMode();
    void onParallelMode();

    void onOverVoltageProtection(bool enabled);
    void onOverCurrentProtection(bool enabled);
    void onOverVoltageProtectionChanged(TChannel channel, double value);
    void onOverCurrentProtectionChanged(TChannel channel, double value);

    void onMemoryKey(TMemoryKey key);

    void onVoltageChanged(TChannel channel, double value);
    void onCurrentChanged(TChannel channel, double value);

public slots:
    void enableMemoryKey(TMemoryKey key);
    void slotDisplayVoltage(TChannel channel, double value);
    void slotDisplayCurrent(TChannel channel, double value);
    void slotUpdateOutputStatus(TOutputStatus channel1, TOutputStatus channel2, bool outputOn);

private slots:
    void slotIndependentMode();
    void slotSerialMode();
    void slotParallelMode();
    void slotMemoryKeyChanged(bool toggle);

    void slotDialControlChanged();
    void slotSpinControlChanged();
    void slotControlValueChanged();
    void slotControlValueChangedDebounced(double value);
    void slotOverProtectionChanged(double value);

private:
    enum {
        HighlightNone = 0,
        HighlightRed = 1,
        HighlightGreen = 2,
    } typedef THighlight;

    Ui::MainWindow *ui;
    ControlDebounce mDebouncedCh1V;
    ControlDebounce mDebouncedCh1A;
    ControlDebounce mDebouncedCh2V;
    ControlDebounce mDebouncedCh2A;

private:
    void showIndependentOutputConfiguration();
    void showSerialOutputConfiguration();
    void showParallelOutputConfiguration();

    void setEnableChannel(TChannel ch, bool enable);
    void showOutputMode(OutputMode mode);

    void openSvg(const QString &resource);

    void highlight(THighlight color, QLabel *label);
};

#endif // MAINWINDOW_H
