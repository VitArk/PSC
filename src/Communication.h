//
// Created by Vitalii Arkusha on 12.09.2022.
//

#ifndef PSC_COMMUNICATION_H
#define PSC_COMMUNICATION_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QTime>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "Commons.h"
#include "Protocol.h"
#include "DeviceStatus.h"

class Communication : public QObject {
    Q_OBJECT
public:
    explicit Communication(QObject *parent = nullptr);
    ~Communication() override = default;
    static QStringList availableSerialPorts();

public slots:
    void openSerialPort(const QString &name, int baudRate);
    void closeSerialPort();

    void lockOperationPanel(bool lock);
    void isOperationPanelLocked();
    void setCurrent(TChannel channel, double value);
    void getCurrent(TChannel channel);
    void setVoltage(TChannel channel, double value);
    void getVoltage(TChannel channel);
    void getOutputCurrent(TChannel channel);
    void getOutputVoltage(TChannel channel);
    void setOutputSwitch(bool ON);
    void enableBeep(bool enable);
    void isBeepEnabled();
    void getDeviceStatus();
    void getDeviceInfo();
    void recallSetting(TMemoryKey key);
    void getActiveSetting();
    void saveSetting(TMemoryKey key);
    void changeOutputConnectionMethod(TOutputConnectionMethod method);
    void enableOverCurrentProtection(bool enable);
    void enableOverVoltageProtection(bool enable);
    void setOverCurrentProtectionValue(TChannel channel, double current);
    void getOverCurrentProtectionValue(TChannel channel);
    void setOverVoltageProtectionValue(TChannel channel, double voltage);
    void getOverVoltageProtectionValue(TChannel channel);

signals:
    void onSerialPortOpened();
    void onSerialPortClosed();
    void onSerialPortError(const QString &error);

    void onOperationPanelLocked(bool locked);
    void onSetCurrent(TChannel channel, double current);
    void onSetVoltage(TChannel channel, double voltage);
    void onOutputCurrent(TChannel channel, double current);
    void onOutputVoltage(TChannel channel, double voltage);
    void onBeepEnabled(bool enabled);
    void onDeviceStatus(DeviceStatus status);
    void onDeviceInfo(const QString &info);
    void onRecalledSetting(TMemoryKey key);
    void onOverCurrentProtectionValue(TChannel channel, double current);
    void onOverVoltageProtectionValue(TChannel channel, double voltage);

private slots:
    void slotSerialReadData();
    void slotProcessRequestQueue();

private:
    void serialSendRequest(bool ignoreDelay = false);
    void dispatchData(const Protocol::IProtocol &request, const QByteArray &data);
    void enqueueRequest(Protocol::IProtocol *request);

private:
    QSerialPort                  mSerialPort;
    QQueue<Protocol::IProtocol*> mRequestQueue;
    QTimer                       mRequestQueueTimer;
    QTime                        mRequestNextTime;
};


#endif //PSC_COMMUNICATION_H
