//
// Created by Vitalii Arkusha on 12.09.2022.
//

#include "Communication.h"
#include "devices/Factory.h"

#include <QTimer>
#include <QDebug>

#define COLLECT_DEBUG_INFO_MS 500

Communication::Communication(QObject *parent) : QObject(parent){
    mSerialPort.setDataBits(QSerialPort::Data8);
    mSerialPort.setParity(QSerialPort::NoParity);
    mSerialPort.setStopBits(QSerialPort::OneStop);
    mSerialPort.setFlowControl(QSerialPort::NoFlowControl);

    mQueueTimer.setInterval(1);
    mQueueTimer.setTimerType(Qt::PreciseTimer);

    connect(&mSerialPort, &QSerialPort::readyRead, this, &Communication::slotSerialReadData, Qt::QueuedConnection);
    connect(&mSerialPort, &QSerialPort::errorOccurred, this, &Communication::slotSerialErrorOccurred);

    connect(&mQueueTimer, &QTimer::timeout, this, &Communication::slotProcessRequestQueue);
    connect(&mDebugTimer, &QTimer::timeout, this, &Communication::slotCollectDebugInfo);
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
        mQueueTimer.start();
        emit onSerialPortOpened(name, baudRate);

        // Request device ID for initialize correct device protocol.
        enqueueMessage(new Protocol::GetDeviceID());
    } else {
        emit onSerialPortErrorOccurred(mSerialPort.errorString());
    }
}

void Communication::closeSerialPort() {
    mQueueTimer.stop();
    while (!mRequestQueue.isEmpty()){
        delete mRequestQueue.dequeue();
    }
    while (!mResponseQueue.isEmpty()){
        delete mResponseQueue.dequeue();
    }

    delete mDeviceProtocol, mDeviceProtocol = nullptr;

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

void Communication::slotProcessRequestQueue() {
    processRequestQueue();
}

void Communication::processRequestQueue(bool ignoreDelay) {
    if (mRequestQueue.isEmpty()) {
        return;
    }

    if (!ignoreDelay && mRequestNextTime > QTime::currentTime()) {
        return;
    }

    auto pMessage = mRequestQueue.dequeue();
    mSerialPort.write(pMessage->query());
    qDebug() << "serial write" << pMessage->query() << "exp resp" << bool(pMessage->answerLength());
    mSerialPort.flush();
    mRequestNextTime = QTime::currentTime().addMSecs(DELAY_BETWEEN_REQUESTS_MS);

    // if response is not expected just remove the message, otherwise move the message into response queue.
    if (pMessage->answerLength() == 0) {
        delete pMessage;
    } else {
        mResponseQueue.enqueue(pMessage);
    }
}

void Communication::slotSerialReadData() {
    if (mResponseQueue.isEmpty()) {
        mSerialPort.clear();
        return;
    }

    auto pMessage = mResponseQueue.head();
    if (mSerialPort.bytesAvailable() >= pMessage->answerLength()) {
        QByteArray buff(mSerialPort.read(pMessage->answerLength()));
        dispatchData(*pMessage, buff);
        delete mResponseQueue.dequeue();

        processRequestQueue(true); // here we are able to send pMessage (process queue) immediately
    }
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
    } else if (typeid(message) == typeid(Protocol::GetActiveSettings)) {
        emit onApplySettings(TMemoryKey(data.toInt(&ok)));
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
        ++mErrorCount;
    }
    qDebug() << "resp data" << data << "is error detected" << !ok;
}

void Communication::createDeviceProtocol(const QByteArray &data) {
    mDeviceProtocol = Protocol::Factory::create(data);
    if (mDeviceProtocol != nullptr) {
        emit onDeviceReady(mDeviceProtocol->info());
    } else {
        emit onUnknownDevice(data);
    }
}

void Communication::enqueueMessage(Protocol::IMessage *message) {
    if (mSerialPort.isOpen()) {
        mRequestQueue.enqueue(message);
    } else {
        delete message;
    }
}

void Communication::enableDebugMode(bool enable) {
    if (enable) {
        mDebugTimer.start(COLLECT_DEBUG_INFO_MS);
    } else {
        mDebugTimer.stop();
    }
}

void Communication::slotCollectDebugInfo() {
    emit onDebugInfoReady(DebugInfo{
        .queueSize = mResponseQueue.length(),
        .errorCount = mErrorCount,
        .currentRequestDelay = mDelayBetweenRequests,
    });
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

void Communication::applySettings(TMemoryKey key) {
    enqueueMessage(mDeviceProtocol->createRequestApplySettings(key));
}

void Communication::getActiveSettings() {
    enqueueMessage(mDeviceProtocol->createRequestGetActiveSettings());
}

void Communication::saveSettings(TMemoryKey key) {
    enqueueMessage(mDeviceProtocol->createRequestSaveSettings(key));
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








