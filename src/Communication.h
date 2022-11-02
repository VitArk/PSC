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

#include "Global.h"
#include "protocol/BaseSCPI.h"
#include "CommunicationMetrics.h"

class Communication : public QObject {
    Q_OBJECT
public:
    explicit Communication(QObject *parent = nullptr);
    ~Communication() override;
signals:
    void onSerialPortOpened(QString serialPortName, int baudRate);
    void onSerialPortClosed();
    void onSerialPortErrorOccurred(QString error);
    void onDeviceReady(Global::DeviceInfo info);
    void onUnknownDevice(QString deviceID);

    void onMetricsReady(const CommunicationMetrics &info);

    void onGetIsLocked(bool locked);
    void onGetCurrentSet(Global::Channel channel, double current);
    void onGetVoltageSet(Global::Channel channel, double voltage);
    void onGetActualCurrent(Global::Channel channel, double current);
    void onGetActualVoltage(Global::Channel channel, double voltage);
    void onGetIsBeepEnabled(bool enabled);
    void onGetDeviceStatus(Global::DeviceStatus status);
    void onGetDeviceID(const QString &info);
    void onGetPreset(Global::MemoryKey key);
    void onGetOverCurrentProtectionValue(Global::Channel channel, double current);
    void onGetOverVoltageProtectionValue(Global::Channel channel, double voltage);

public slots:
    void OpenSerialPort(const QString &name, int baudRate);
    void CloseSerialPort();

    void SetLocked(bool lock);
    void GetIsLocked();
    void SetCurrent(Global::Channel channel, double value);
    void GetCurrentSet(Global::Channel channel);
    void SetVoltage(Global::Channel channel, double value);
    void GetVoltageSet(Global::Channel channel);
    void GetActualCurrent(Global::Channel channel);
    void GetActualVoltage(Global::Channel channel);
    void SetEnableOutputSwitch(bool enable);
    void SetEnableBeep(bool enable);
    void GetIsBuzzerEnabled();
    void GetDeviceStatus();
    void GetDeviceID();
    void SetPreset(Global::MemoryKey key);
    void GetPreset();
    void SavePreset(Global::MemoryKey key);
    void SetChannelTracking(Global::ChannelsTracking mode);
    void SetEnableOverCurrentProtection(bool enable);
    void SetEnableOverVoltageProtection(bool enable);
    void SetOverCurrentProtectionValue(Global::Channel channel, double current);
    void GetOverCurrentProtectionValue(Global::Channel channel);
    void SetOverVoltageProtectionValue(Global::Channel channel, double voltage);
    void GetOverVoltageProtectionValue(Global::Channel channel);

private slots:
    void SerialPortReadyRead();
    void SerialPortErrorOccurred(QSerialPort::SerialPortError error);
    void SerialPortReplyTimeout();

    void CollectMetrics();

private:
    void processMessageQueue(bool clearBusyFlag);
    void dispatchMessageReplay(const Protocol::IMessage &message, const QByteArray &reply);
    void enqueueMessage(Protocol::IMessage *pMessage);
    bool isQueueOverflow() const;

private:
    QSerialPort                  mSerialPort;
    QQueue<Protocol::IMessage*>  mMessageQueue;
    QTimer                       mWaitResponseTimer;
    volatile bool                mIsBusy = false;
    Protocol::BaseSCPI*          mDeviceProtocol = nullptr;

    QTimer                       mMetricCollectorTimer;
    CommunicationMetrics         mMetrics;
};


#endif //PSC_COMMUNICATION_H
