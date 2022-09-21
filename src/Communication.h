//
// Created by Vitalii Arkusha on 12.09.2022.
//

#ifndef PSC_COMMUNICATION_H
#define PSC_COMMUNICATION_H

#include <QObject>
#include <QQueue>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "Commons.h"
#include "Protocol.h"

class Communication : public QObject {
    Q_OBJECT
public:
    explicit Communication(QObject *parent = nullptr);
    ~Communication() override = default;
    static QStringList availableSerialPorts();

public slots:
    void openSerialPort(const QString &name, int baudRate);
    void closeSerialPort();

    void LockOperationPanel(bool state);
    void SetCurrent(TChannel channel, double value);
    void ReadSetCurrent(TChannel channel);
    void SetVoltage(TChannel channel, double value);
    void ReadSetVoltage(TChannel channel);
    void ReadOutputCurrent(TChannel channel);
    void ReadOutputVoltage(TChannel channel);
    void Output(bool state);
    void Buzzer(bool state);
    void ReadOutputStatus();
    void ReadDeviceInfo();
    void RecallSetting(TMemoryKey key);
    void ReadActiveSetting();
    void SaveSetting(TMemoryKey key);
    void OutputMode(TOutputMode mode);
    void OverCurrentProtection(bool state);
    void OverVoltageProtection(bool state);
    void SetOverCurrentProtection(TChannel channel, double value);
    void SetOverVoltageProtection(TChannel channel, double value);

signals:
    void onSerialPortOpened();
    void onSerialPortClosed();
    void onSerialPortError(const QString &error);


    void onSetCurrent(TChannel channel, double value);
    void onSetVoltage(TChannel channel, double value);
    void onOutputCurrent(TChannel channel, double value);
    void onOutputVoltage(TChannel channel, double value);
    void onOutputStatus(TOutputStatus channel1, TOutputStatus channel2, bool outputOn);
    void onDeviceInfo(const QString &info);
    void onActiveSetting(TMemoryKey key);

private slots:
    void serialReadData();
    void serialSendRequest();

private:
    void dispatchData(const Protocol::IProtocol &request, const QByteArray &data);
    void enqueueRequest(Protocol::IProtocol *request);

private:
    QSerialPort                  mSerialPort;
    QQueue<Protocol::IProtocol*> mRequestQueue;
};


#endif //PSC_COMMUNICATION_H
