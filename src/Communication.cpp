//
// Created by Vitalii Arkusha on 12.09.2022.
//

#include "Communication.h"
#include "devices/Factory.h"

#include <QTimer>
#include <QThread>
#include <QDebug>

#define COLLECT_DEBUG_INFO_MS 500
#define DELAY_BETWEEN_REQUESTS_MS 60

Communication::Communication(QObject *parent) : QObject(parent){
    mSerialPort.setDataBits(QSerialPort::Data8);
    mSerialPort.setParity(QSerialPort::NoParity);
    mSerialPort.setStopBits(QSerialPort::OneStop);
    mSerialPort.setFlowControl(QSerialPort::NoFlowControl);

    connect(&mSerialPort, &QSerialPort::readyRead, this, &Communication::slotSerialReadData);
    connect(&mSerialPort, &QSerialPort::errorOccurred, this, &Communication::slotSerialErrorOccurred);

    mMetricCollectorTimer.start(COLLECT_DEBUG_INFO_MS);
    connect(&mMetricCollectorTimer, &QTimer::timeout, this, &Communication::slotCollectMetrics);

    mWaitResponseTimer.setSingleShot(true);
    connect(&mWaitResponseTimer, &QTimer::timeout, this, &Communication::slotResponseTimeout);
}

Communication::~Communication() {
    closeSerialPort();
}


void Communication::openSerialPort(const QString &name, int baudRate) {
    closeSerialPort();
    mSerialPort.setPortName(name);
    mSerialPort.setBaudRate(baudRate);

    if (mSerialPort.open(QIODevice::ReadWrite)) {
        mSerialPort.clear();
        emit onSerialPortOpened(name, baudRate);

        mIsBusy = false;
        // Request device ID for initialize correct device protocol.
        enqueueMessage(new Protocol::GetDeviceID()); // TODO: the request should use blocked method (waitForRead)
    } else {
        emit onSerialPortErrorOccurred(mSerialPort.errorString());
    }
}

void Communication::closeSerialPort() {
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

void Communication::slotSerialErrorOccurred(QSerialPort::SerialPortError error) {
    if (error != QSerialPort::NoError) {
        QString errorString = mSerialPort.errorString();
        closeSerialPort();
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
    //qDebug() << "send query " << pMessage->query();

    // if the message is command (response is not expected), just remove the message from queue
    // and give some time for execute the action on the devise.
    if (pMessage->isCommand()) {
        delete mMessageQueue.dequeue();
        QTimer::singleShot(DELAY_BETWEEN_REQUESTS_MS, this, [this] () {
            processMessageQueue(true);
        });
    } else {
        mWaitResponseTimer.start(DELAY_BETWEEN_REQUESTS_MS * 2);
    }
}

void Communication::slotSerialReadData() {
    if (mMessageQueue.isEmpty()) {
        mSerialPort.clear();
        return;
    }

    auto pMessage = mMessageQueue.head();
    if (mSerialPort.bytesAvailable() >= pMessage->answerLength()) {
        mWaitResponseTimer.stop();

        QByteArray buff(mSerialPort.read(pMessage->answerLength()));
        dispatchData(*pMessage, buff);
        delete mMessageQueue.dequeue();

        processMessageQueue(true);
    }
}

void Communication::slotResponseTimeout() {
    mMetrics.responseTimeoutCount++;
    mMessageQueue.clear();
    mSerialPort.clear();
    mIsBusy = false;
}


void Communication::dispatchData(const Protocol::IMessage &message, const QByteArray &data) {
    //qDebug() << "resp data" << data;
    bool ok = true;
    if (typeid(message) == typeid(Protocol::GetDeviceStatus)) {
        emit onDeviceStatus(DeviceStatus(data.at(0)));
    } else if (typeid(message) == typeid(Protocol::GetOutputCurrent)) {
        emit onOutputCurrent(message.channel(), data.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::GetOutputVoltage)) {
        emit onOutputVoltage(message.channel(), data.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::GetCurrent)) {
        emit onSetCurrent(message.channel(), data.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::GetVoltage)) {
        emit onSetVoltage(message.channel(), data.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::GetOverCurrentProtectionValue)) {
        emit onOverCurrentProtectionValue(message.channel(), data.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::GetOverVoltageProtectionValue)) {
        emit onOverVoltageProtectionValue(message.channel(), data.toDouble(&ok));
    } else if (typeid(message) == typeid(Protocol::GetPreset)) {
        emit onActiveSettings(TMemoryKey(data.toInt(&ok)));
    } else if (typeid(message) == typeid(Protocol::IsOperationPanelLocked)) {
        emit onOperationPanelLocked(bool(data.toInt(&ok)));
    } else if (typeid(message) == typeid(Protocol::IsBuzzerEnabled)) {
        emit onBuzzerEnabled(bool(data.toInt(&ok)));
    } else if (typeid(message) == typeid(Protocol::GetDeviceID)) {
        if (mDeviceProtocol != nullptr) {
            emit onDeviceInfo(data);
        } else {
            createDeviceProtocol(data);
        }
    } else {
        qDebug() << "Unknown response (message)" << data;
    }

    if (!ok) {
        mMetrics.errorCount++;
    }
}

void Communication::createDeviceProtocol(const QByteArray &data) {
    mDeviceProtocol = Protocol::Factory::create(data);
    if (mDeviceProtocol != nullptr) {
        emit onDeviceReady(mDeviceProtocol->info());
    } else {
        emit onUnknownDevice(data);
    }
}

bool Communication::isQueueOverflow() const {
    return mMetrics.queueSize() > 5;
}

void Communication::enqueueMessage(Protocol::IMessage *pMessage) {
    if (mSerialPort.isOpen()) {
        if (isQueueOverflow() && pMessage->allowDrop()) {
            mMetrics.droppedCount++;
            delete pMessage;
            return;
        }

        if (pMessage->isQuery() || mMessageQueue.length() < 2) {
            mMessageQueue.enqueue(pMessage);
        } else {
            mMessageQueue.insert(1, pMessage); // bring priority for command message.
        }
    } else {
        delete pMessage;
    }
    processMessageQueue(false);
}

void Communication::slotCollectMetrics() {
    mMetrics.setQueueSize(mMessageQueue.length());
    emit onMetricsReady(mMetrics);
}

void Communication::lockOperationPanel(bool lock) {
    enqueueMessage(mDeviceProtocol->createRequestLockOperationPanel(lock));
}

void Communication::isOperationPanelLocked() {
    enqueueMessage(mDeviceProtocol->createRequestIsOperationPanelLocked());
}

void Communication::setCurrent(TChannel channel, double value) {
    enqueueMessage(mDeviceProtocol->createRequestSetCurrent(channel, value));
}

void Communication::getCurrent(TChannel channel) {
    enqueueMessage(mDeviceProtocol->createRequestGetCurrent(channel));
}

void Communication::setVoltage(TChannel channel, double value) {
    enqueueMessage(mDeviceProtocol->createRequestSetVoltage(channel, value));
}

void Communication::getVoltage(TChannel channel) {
    enqueueMessage(mDeviceProtocol->createRequestGetVoltage(channel));
}

void Communication::getOutputCurrent(TChannel channel) {
    enqueueMessage(mDeviceProtocol->createRequestGetOutputCurrent(channel));
}

void Communication::getOutputVoltage(TChannel channel) {
    enqueueMessage(mDeviceProtocol->createRequestGetOutputVoltage(channel));
}

void Communication::setEnableOutputSwitch(bool enable) {
    enqueueMessage(mDeviceProtocol->createRequestEnableOutputSwitch(enable));
}

void Communication::enableBuzzer(bool enable) {
    enqueueMessage(mDeviceProtocol->createRequestEnableBuzzer(enable));
}

void Communication::isBuzzerEnabled() {
    enqueueMessage(mDeviceProtocol->createRequestIsBuzzerEnabled());
}

void Communication::getDeviceStatus() {
    enqueueMessage(mDeviceProtocol->createRequestGetDeviceStatus());
}

void Communication::getDeviceID() {
    enqueueMessage(mDeviceProtocol->createRequestGetDeviceID());
}

void Communication::setPreset(TMemoryKey key) {
    enqueueMessage(mDeviceProtocol->createRequestSetPreset(key));
}

void Communication::getPreset() {
    enqueueMessage(mDeviceProtocol->createRequestGetPreset());
}

void Communication::savePreset(TMemoryKey key) {
    enqueueMessage(mDeviceProtocol->createRequestSavePreset(key));
}

void Communication::changeOutputConnectionMethod(TOutputConnectionMethod method) {
    enqueueMessage(mDeviceProtocol->createRequestChangeOutputConnectionMethod(method));
}

void Communication::enableOverCurrentProtection(bool enable) {
    enqueueMessage(mDeviceProtocol->createRequestEnableOverCurrentProtection(enable));
}

void Communication::enableOverVoltageProtection(bool enable) {
    enqueueMessage(mDeviceProtocol->createRequestEnableOverVoltageProtection(enable));
}

void Communication::setOverCurrentProtectionValue(TChannel channel, double current) {
    enqueueMessage(mDeviceProtocol->createRequestSetOverCurrentProtectionValue(channel, current));
}

void Communication::getOverCurrentProtectionValue(TChannel channel) {
    enqueueMessage(mDeviceProtocol->createRequestGetOverCurrentProtectionValue(channel));
}

void Communication::setOverVoltageProtectionValue(TChannel channel, double voltage) {
    enqueueMessage(mDeviceProtocol->createRequestSetOverVoltageProtectionValue(channel, voltage));
}

void Communication::getOverVoltageProtectionValue(TChannel channel) {
    enqueueMessage(mDeviceProtocol->createRequestGetOverVoltageProtectionValue(channel));
}










