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
#include "protocol/Interface.h"
#include "DeviceStatus.h"
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
    void onDeviceReady(DeviceInfo info);
    void onUnknownDevice(QString deviceID);
    void onMetricsReady(const CommunicationMetrics &info);

    void onOperationPanelLocked(bool locked);
    void onSetCurrent(TChannel channel, double current);
    void onSetVoltage(TChannel channel, double voltage);
    void onOutputCurrent(TChannel channel, double current);
    void onOutputVoltage(TChannel channel, double voltage);
    void onBuzzerEnabled(bool enabled);
    void onDeviceStatus(DeviceStatus status);
    void onDeviceInfo(const QString &info);
    void onActiveSettings(TMemoryKey key);
    void onOverCurrentProtectionValue(TChannel channel, double current);
    void onOverVoltageProtectionValue(TChannel channel, double voltage);

public slots:
    void openSerialPort(const QString &name, int baudRate);
    void closeSerialPort();

    void setLocked(bool lock);
    void getIsLocked();
    void setCurrent(TChannel channel, double value);
    void getCurrentSet(TChannel channel);
    void setVoltage(TChannel channel, double value);
    void getVoltageSet(TChannel channel);
    void getActualCurrent(TChannel channel);
    void getActualVoltage(TChannel channel);
    void setEnableOutputSwitch(bool enable);
    void setEnableBuzzer(bool enable);
    void getIsBuzzerEnabled();
    void getDeviceStatus();
    void getDeviceID();
    void setPreset(TMemoryKey key);
    void getPreset();
    void savePreset(TMemoryKey key);
    void setChannelTracking(TChannelTracking mode);
    void setEnableOverCurrentProtection(bool enable);
    void setEnableOverVoltageProtection(bool enable);
    void setOverCurrentProtectionValue(TChannel channel, double current);
    void getOverCurrentProtectionValue(TChannel channel);
    void setOverVoltageProtectionValue(TChannel channel, double voltage);
    void getOverVoltageProtectionValue(TChannel channel);

private slots:
    void slotSerialReadData();
    void slotSerialErrorOccurred(QSerialPort::SerialPortError error);
    void slotCollectMetrics();

    void slotResponseTimeout();

private:
    void processMessageQueue(bool clearBusyFlag);
    void dispatchMessageReplay(const Protocol::IMessage &message, const QByteArray &reply);
    void enqueueMessage(Protocol::IMessage *pMessage);
    bool isQueueOverflow() const;

private:
    QSerialPort                  mSerialPort;
    QQueue<Protocol::IMessage*>  mMessageQueue;
    QTimer                       mWaitResponseTimer;

    Protocol::Interface*         mDeviceProtocol = nullptr;
    QTimer                       mMetricCollectorTimer;
    CommunicationMetrics         mMetrics;
    volatile bool                mIsBusy = false;
};


#endif //PSC_COMMUNICATION_H
