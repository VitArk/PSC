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

#include "protocol//Commons.h"
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
    void onDeviceReady(Protocol::DeviceInfo info);
    void onUnknownDevice(QString deviceID);

    void onMetricsReady(const CommunicationMetrics &info);

    void onGetIsLocked(bool locked);
    void onGetCurrentSet(Protocol::Channel channel, double current);
    void onGetVoltageSet(Protocol::Channel channel, double voltage);
    void onGetActualCurrent(Protocol::Channel channel, double current);
    void onGetActualVoltage(Protocol::Channel channel, double voltage);
    void onGetIsBeepEnabled(bool enabled);
    void onGetDeviceStatus(Protocol::DeviceStatus status);
    void onGetDeviceID(const QString &info);
    void onGetPreset(Protocol::MemoryKey key);
    void onGetOverCurrentProtectionValue(Protocol::Channel channel, double current);
    void onGetOverVoltageProtectionValue(Protocol::Channel channel, double voltage);

public slots:
    void openSerialPort(const QString &name, int baudRate);
    void closeSerialPort();

    void setLocked(bool lock);
    void getIsLocked();
    void setCurrent(Protocol::Channel channel, double value);
    void getCurrentSet(Protocol::Channel channel);
    void setVoltage(Protocol::Channel channel, double value);
    void getVoltageSet(Protocol::Channel channel);
    void getActualCurrent(Protocol::Channel channel);
    void getActualVoltage(Protocol::Channel channel);
    void setEnableOutputSwitch(bool enable);
    void setEnableBuzzer(bool enable);
    void getIsBuzzerEnabled();
    void getDeviceStatus();
    void getDeviceID();
    void setPreset(Protocol::MemoryKey key);
    void getPreset();
    void savePreset(Protocol::MemoryKey key);
    void setChannelTracking(Protocol::ChannelTracking mode);
    void setEnableOverCurrentProtection(bool enable);
    void setEnableOverVoltageProtection(bool enable);
    void setOverCurrentProtectionValue(Protocol::Channel channel, double current);
    void getOverCurrentProtectionValue(Protocol::Channel channel);
    void setOverVoltageProtectionValue(Protocol::Channel channel, double voltage);
    void getOverVoltageProtectionValue(Protocol::Channel channel);

private slots:
    void serialPortReadData();
    void serialPortErrorOccurred(QSerialPort::SerialPortError error);
    void serialPortReplyTimeout();

    void collectMetrics();

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
