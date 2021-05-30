#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    void onMemoryKey(TMemoryKey key);

    void onVoltageChanged(TChannel channel, double value);
    void onCurrentChanged(TChannel channel, double value);

public slots:
    void enableMemoryKey(TMemoryKey key);

private slots:
    void slotIndependentMode();
    void slotSerialMode();
    void slotParallelMode();
    void slotMemoryKeyChanged(bool toggle);

    void slotDialControlChanged();
    void slotSpinControlChanged();
    void slotControlValueChanged();
    void slotControlValueChangedDebounced(double value);

private:
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
};

#endif // MAINWINDOW_H
