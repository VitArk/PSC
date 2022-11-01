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

#include "Communication.h"
#include "protocol/Factory.h"

#include <QTimer>

#define COLLECT_DEBUG_INFO_MS 500
#define DELAY_BETWEEN_REQUESTS_MS 60

Communication::Communication(QObject *parent) : QObject(parent){
    mSerialPort.setDataBits(QSerialPort::Data8);
    mSerialPort.setParity(QSerialPort::NoParity);
    mSerialPort.setStopBits(QSerialPort::OneStop);
    mSerialPort.setFlowControl(QSerialPort::NoFlowControl);

    connect(&mSerialPort, &QSerialPort::readyRead, this, &Communication::SerialPortReadyRead);
    connect(&mSerialPort, &QSerialPort::errorOccurred, this, &Communication::SerialPortErrorOccurred);

    mMetricCollectorTimer.start(COLLECT_DEBUG_INFO_MS);
    connect(&mMetricCollectorTimer, &QTimer::timeout, this, &Communication::CollectMetrics);

    mWaitResponseTimer.setSingleShot(true);
    connect(&mWaitResponseTimer, &QTimer::timeout, this, &Communication::SerialPortReplyTimeout);
}

Communication::~Communication() {
    CloseSerialPort();
}

void Communication::OpenSerialPort(const QString &name, int baudRate) {
    CloseSerialPort();
    mSerialPort.setPortName(name);
    mSerialPort.setBaudRate(baudRate);

    if (mSerialPort.open(QIODevice::ReadWrite)) {
        mSerialPort.clear();
        emit onSerialPortOpened(name, baudRate);

        // The instance of Protocol::Factory is blocking all QT signals of QSerialPort (mSerialPort) until be destroyed.
        auto factory = Protocol::Factory(mSerialPort);
        mDeviceProtocol = factory.createInstance();
        if (mDeviceProtocol != nullptr) {
            emit onDeviceReady(mDeviceProtocol->deviceInfo());
        } else {
            if (factory.deviceID().isEmpty()) {
                emit onSerialPortErrorOccurred(factory.errorString());
            } else {
                emit onUnknownDevice(factory.deviceID());
            }
        }
    } else {
        emit onSerialPortErrorOccurred(mSerialPort.errorString());
    }
}

void Communication::CloseSerialPort() {
    while (!mMessageQueue.isEmpty()){
        delete mMessageQueue.dequeue();
    }

    delete mDeviceProtocol, mDeviceProtocol = nullptr;

    mMetrics = CommunicationMetrics();

    if (mSerialPort.isOpen()) {
        mSerialPort.close();
        emit onSerialPortClosed();
    }
}

void Communication::SerialPortErrorOccurred(QSerialPort::SerialPortError error) {
    if (error != QSerialPort::NoError) {
        QString errorString = mSerialPort.errorString();
        CloseSerialPort();
        mSerialPort.clearError();

        emit onSerialPortErrorOccurred(errorString);
    }
}

void Communication::processMessageQueue(bool clearBusyFlag) {
    if (clearBusyFlag) {
        mIsBusy = false;
    }

    if (mIsBusy || mMessageQueue.isEmpty()) {
        return;
    }

    mIsBusy = true;
    auto pMessage = mMessageQueue.head();
    mSerialPort.write(pMessage->query());
    mSerialPort.flush();

    // if the message is command (response is not expected), just remove the message from queue
    // and give some time for execute the action on the devise.
    if (!pMessage->isCommandWithReply()) {
        delete mMessageQueue.dequeue();
        QTimer::singleShot(DELAY_BETWEEN_REQUESTS_MS, this, [this] () {
            processMessageQueue(true);
        });
    } else {
        mWaitResponseTimer.start(DELAY_BETWEEN_REQUESTS_MS * 2);
    }
}

void Communication::SerialPortReadyRead() {
    if (mMessageQueue.isEmpty()) {
        mSerialPort.clear();
        return;
    }

    auto pMessage = mMessageQueue.head();
    if (mSerialPort.bytesAvailable() >= pMessage->replySize()) {
        mWaitResponseTimer.stop();

        QByteArray reply(mSerialPort.read(pMessage->replySize()));
        dispatchMessageReplay(*pMessage, reply);
        delete mMessageQueue.dequeue();

        processMessageQueue(true);
    }
}

void Communication::SerialPortReplyTimeout() {
    mMetrics.responseTimeoutCount++;
    mMessageQueue.clear();
    mSerialPort.clear();
    mIsBusy = false;
}

bool Communication::isQueueOverflow() const {
    return mMetrics.queueLength() > 5;
}

void Communication::enqueueMessage(Protocol::IMessage *pMessage) {
    if (mSerialPort.isOpen()) {
        if (isQueueOverflow() && pMessage->allowToDrop()) {
            mMetrics.droppedCount++;
            delete pMessage;
            return;
        }

        if (pMessage->isCommandWithReply() || mMessageQueue.length() < 2) {
            mMessageQueue.enqueue(pMessage);
        } else {
            mMessageQueue.insert(1, pMessage); // bring priority for command message.
        }
    } else {
        delete pMessage;
    }
    processMessageQueue(false);
}

void Communication::CollectMetrics() {
    mMetrics.setQueueLength(mMessageQueue.length());
    emit onMetricsReady(mMetrics);
}

void Communication::dispatchMessageReplay(const Protocol::IMessage &message, const QByteArray &reply) {
    bool ok = true;
    if (typeid(message) == typeid(Protocol::MessageGetDeviceStatus)) {
        //mDeviceProtocol->processDeviceStatusReply(reply);
        emit onGetDeviceStatus(mDeviceProtocol->processDeviceStatusReply(reply));
    } else if (typeid(message) == typeid(Protocol::MessageGetActualCurrent)) {
        emit onGetActualCurrent(message.channel(), reply.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::MessageGetActualVoltage)) {
        emit onGetActualVoltage(message.channel(), reply.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::MessageGetCurrentSet)) {
        emit onGetCurrentSet(message.channel(), reply.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::MessageGetVoltageSet)) {
        emit onGetVoltageSet(message.channel(), reply.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::MessageGetOverCurrentProtectionValue)) {
        emit onGetOverCurrentProtectionValue(message.channel(), reply.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::MessageGetOverVoltageProtectionValue)) {
        emit onGetOverVoltageProtectionValue(message.channel(), reply.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::MessageGetPreset)) {
        emit onGetPreset(Global::MemoryKey(reply.toInt(&ok)));
    } else if (typeid(message) == typeid(Protocol::MessageGetIsLocked)) {
        emit onGetIsLocked(bool(reply.toInt(&ok)));
    } else if (typeid(message) == typeid(Protocol::MessageGetIsBeepEnabled)) {
        emit onGetIsBeepEnabled(bool(reply.toInt(&ok)));
    } else if (typeid(message) == typeid(Protocol::MessageGetDeviceID)) {
        emit onGetDeviceID(reply);
    } else {
        qDebug() << "Unknown message reply" << reply;
    }

    if (!ok) {
        mMetrics.errorCount++;
    }
}

void Communication::SetLocked(bool lock) {
    enqueueMessage(mDeviceProtocol->createMessageSetLocked(lock));
}

void Communication::GetIsLocked() {
    enqueueMessage(mDeviceProtocol->createMessageGetIsLocked());
}

void Communication::SetCurrent(Global::Channel channel, double value) {
    enqueueMessage(mDeviceProtocol->createMessageSetCurrent(channel, value));
}

void Communication::GetCurrentSet(Global::Channel channel) {
    enqueueMessage(mDeviceProtocol->createMessageGetCurrentSet(channel));
}

void Communication::SetVoltage(Global::Channel channel, double value) {
    enqueueMessage(mDeviceProtocol->createMessageSetVoltage(channel, value));
}

void Communication::GetVoltageSet(Global::Channel channel) {
    enqueueMessage(mDeviceProtocol->createMessageGetVoltageSet(channel));
}

void Communication::GetActualCurrent(Global::Channel channel) {
    enqueueMessage(mDeviceProtocol->createMessageGetActualCurrent(channel));
}

void Communication::GetActualVoltage(Global::Channel channel) {
    enqueueMessage(mDeviceProtocol->createMessageGetActualVoltage(channel));
}

void Communication::SetEnableOutputSwitch(bool enable) {
    enqueueMessage(mDeviceProtocol->createMessageSetEnableOutputSwitch(enable));
}

void Communication::SetEnableBeep(bool enable) {
    enqueueMessage(mDeviceProtocol->createMessageSetEnableBeep(enable));
}

void Communication::GetIsBuzzerEnabled() {
    enqueueMessage(mDeviceProtocol->createMessageGetIsBeepEnabled());
}

void Communication::GetDeviceStatus() {
    enqueueMessage(mDeviceProtocol->createMessageGetDeviceStatus());
}

void Communication::GetDeviceID() {
    enqueueMessage(mDeviceProtocol->createMessageGetDeviceID());
}

void Communication::SetPreset(Global::MemoryKey key) {
    enqueueMessage(mDeviceProtocol->createMessageSetPreset(key));
}

void Communication::GetPreset() {
    enqueueMessage(mDeviceProtocol->createMessageGetPreset());
}

void Communication::SavePreset(Global::MemoryKey key) {
    enqueueMessage(mDeviceProtocol->createMessageSavePreset(key));
}

void Communication::SetChannelTracking(Global::ChannelTracking mode) {
    enqueueMessage(mDeviceProtocol->createMessageSetChannelTracking(mode));
}

void Communication::SetEnableOverCurrentProtection(bool enable) {
    enqueueMessage(mDeviceProtocol->createMessageSetEnableOverCurrentProtection(enable));
}

void Communication::SetEnableOverVoltageProtection(bool enable) {
    enqueueMessage(mDeviceProtocol->createMessageSetEnableOverVoltageProtection(enable));
}

void Communication::SetOverCurrentProtectionValue(Global::Channel channel, double current) {
    enqueueMessage(mDeviceProtocol->createMessageSetOverCurrentProtectionValue(channel, current));
}

void Communication::GetOverCurrentProtectionValue(Global::Channel channel) {
    enqueueMessage(mDeviceProtocol->createMessageGetOverCurrentProtectionValue(channel));
}

void Communication::SetOverVoltageProtectionValue(Global::Channel channel, double voltage) {
    enqueueMessage(mDeviceProtocol->createMessageSetOverVoltageProtectionValue(channel, voltage));
}

void Communication::GetOverVoltageProtectionValue(Global::Channel channel) {
    enqueueMessage(mDeviceProtocol->createMessageGetOverVoltageProtectionValue(channel));
}
