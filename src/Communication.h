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

#include "devices/Commons.h"
#include "devices/Device.h"
#include "DeviceStatus.h"

class Communication : public QObject {
    Q_OBJECT
public:
    explicit Communication(QObject *parent = nullptr);
    ~Communication() override;
signals:
    void onSerialPortOpened(QString serialPortName, int baudRate);
    void onSerialPortClosed();
    void onSerialPortErrorOccurred(QString error);
    void onSerialPortReady(DeviceInfo info);
    void onUnknownDevice(QString deviceID);

    void onOperationPanelLocked(bool locked);
    void onSetCurrent(TChannel channel, double current);
    void onSetVoltage(TChannel channel, double voltage);
    void onOutputCurrent(TChannel channel, double current);
    void onOutputVoltage(TChannel channel, double voltage);
    void onBuzzerEnabled(bool enabled);
    void onDeviceStatus(DeviceStatus status);
    void onDeviceInfo(const QString &info);
    void onApplySettings(TMemoryKey key);
    void onOverCurrentProtectionValue(TChannel channel, double current);
    void onOverVoltageProtectionValue(TChannel channel, double voltage);

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
    void setEnableOutputSwitch(bool enable);
    void enableBuzzer(bool enable);
    void isBuzzerEnabled();
    void getDeviceStatus();
    void getDeviceID();
    void applySettings(TMemoryKey key);
    void getActiveSettings();
    void saveSettings(TMemoryKey key);
    void changeOutputConnectionMethod(TOutputConnectionMethod method);
    void enableOverCurrentProtection(bool enable);
    void enableOverVoltageProtection(bool enable);
    void setOverCurrentProtectionValue(TChannel channel, double current);
    void getOverCurrentProtectionValue(TChannel channel);
    void setOverVoltageProtectionValue(TChannel channel, double voltage);
    void getOverVoltageProtectionValue(TChannel channel);

private slots:
    void slotSerialReadData();
    void slotSerialErrorOccurred(QSerialPort::SerialPortError error);
    void slotProcessRequestQueue();

private:
    void serialSendMessage(bool ignoreDelay = false);
    void dispatchData(const Protocol::IMessage &message, const QByteArray &data);
    void enqueueMessage(Protocol::IMessage *message);
    void createDeviceProtocol(const QByteArray &data);

private:
    QSerialPort                  mSerialPort;
    QQueue<Protocol::IMessage*>  mMessageQueue;
    QTimer                       mQueueTimer;
    QTime                        mRequestNextTime;
    Protocol::Device*            mDeviceProtocol = nullptr;


};


#endif //PSC_COMMUNICATION_H
