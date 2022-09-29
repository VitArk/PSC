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
    void enableBuzzer(bool enable);
    void isBuzzerEnabled();
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
    void onSerialPortErrorOccurred(QString error);

    void onOperationPanelLocked(bool locked);
    void onSetCurrent(TChannel channel, double current);
    void onSetVoltage(TChannel channel, double voltage);
    void onOutputCurrent(TChannel channel, double current);
    void onOutputVoltage(TChannel channel, double voltage);
    void onBuzzerEnabled(bool enabled);
    void onDeviceStatus(DeviceStatus status);
    void onDeviceInfo(const QString &info);
    void onRecalledSetting(TMemoryKey key);
    void onOverCurrentProtectionValue(TChannel channel, double current);
    void onOverVoltageProtectionValue(TChannel channel, double voltage);

private slots:
    void slotSerialReadData();
    void slotSerialErrorOccurred(QSerialPort::SerialPortError error);
    void slotProcessRequestQueue();

private:
    void serialSendRequest(bool ignoreDelay = false);
    void dispatchData(const Protocol::IProtocol &request, const QByteArray &data);
    void enqueueRequest(Protocol::IProtocol *request);

private:
    QSerialPort                  mSerialPort;
    QQueue<Protocol::IProtocol*> mRequestQueue;
    QTimer                       mQueueTimer;
    QTime                        mRequestNextTime;
};


#endif //PSC_COMMUNICATION_H
