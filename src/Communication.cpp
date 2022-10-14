//
// Created by Vitalii Arkusha on 12.09.2022.
//

#include "Communication.h"
#include "devices/Factory.h"

#include <QTimer>
#include <QDebug>

#define DELAY_BETWEEN_REQUEST_MS 40

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
        enqueueRequest(new Protocol::GetDeviceID());
    } else {
        emit onSerialPortErrorOccurred(mSerialPort.errorString());
    }
}

void Communication::closeSerialPort() {
    mQueueTimer.stop();
    while (!mRequestQueue.isEmpty()){
        delete mRequestQueue.dequeue();
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
    serialSendRequest();
}

void Communication::slotSerialReadData() {
    if (mRequestQueue.isEmpty()) {
        mSerialPort.clear();
        return;
    }

    auto request = mRequestQueue.head();
    if (mSerialPort.bytesAvailable() >= request->answerLength()) {
        QByteArray buff(mSerialPort.read(request->answerLength()));
        dispatchData(*request, buff);
        delete mRequestQueue.dequeue();

        serialSendRequest(true); // here we are able to send request (process queue) immediately
    }
}

void Communication::dispatchData(const Protocol::IRequest &request, const QByteArray &data) {
    if (typeid(request) == typeid(Protocol::GetDeviceStatus)) {
        emit onDeviceStatus(DeviceStatus(data.at(0)));
    } else if (typeid(request) == typeid(Protocol::GetOutputCurrent)) {
        emit onOutputCurrent(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::GetOutputVoltage)) {
        emit onOutputVoltage(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::GetCurrent)) {
        emit onSetCurrent(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::GetVoltage)) {
        emit onSetVoltage(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::GetOverCurrentProtectionValue)) {
        emit onOverCurrentProtectionValue(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::GetOverVoltageProtectionValue)) {
        emit onOverVoltageProtectionValue(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::GetActiveSettings)) {
        emit onApplySettings(TMemoryKey(data.toInt()));
    } else if (typeid(request) == typeid(Protocol::IsOperationPanelLocked)) {
        emit onOperationPanelLocked(bool(data.toInt()));
    } else if (typeid(request) == typeid(Protocol::IsBuzzerEnabled)) {
        emit onBuzzerEnabled(bool(data.toInt()));
    } else if (typeid(request) == typeid(Protocol::GetDeviceID)) {
        if (mDeviceProtocol != nullptr) {
            emit onDeviceInfo(data);
        } else {
            createDeviceProtocol(data);
        }
    }
}

void Communication::createDeviceProtocol(const QByteArray &data) {
    mDeviceProtocol = Protocol::Factory::create(data);
    if (mDeviceProtocol != nullptr) {
        emit onSerialPortReady(mDeviceProtocol->info());
    } else {
        emit onUnknownDevice(data);
    }
}

void Communication::serialSendRequest(bool ignoreDelay) {
    if (mRequestQueue.isEmpty()) {
        return;
    }

    if (!ignoreDelay && mRequestNextTime > QTime::currentTime()) {
        return;
    }

    auto request = mRequestQueue.head();
    mSerialPort.write(request->query());
    mSerialPort.flush();
    mRequestNextTime = QTime::currentTime().addMSecs(DELAY_BETWEEN_REQUEST_MS);

    if (request->answerLength() == 0) { // not expect response, remove
        delete mRequestQueue.dequeue();
    }
}

void Communication::enqueueRequest(Protocol::IRequest *request) {
    if (mSerialPort.isOpen()) {
        mRequestQueue.enqueue(request);
    } else {
        delete request;
    }
}

void Communication::lockOperationPanel(bool lock) {
    enqueueRequest(mDeviceProtocol->createRequestLockOperationPanel(lock));
}

void Communication::isOperationPanelLocked() {
    enqueueRequest(mDeviceProtocol->createRequestIsOperationPanelLocked());
}

void Communication::setCurrent(TChannel channel, double value) {
    enqueueRequest(mDeviceProtocol->createRequestSetCurrent(channel, value));
}

void Communication::getCurrent(TChannel channel) {
    enqueueRequest(mDeviceProtocol->createRequestGetCurrent(channel));
}

void Communication::setVoltage(TChannel channel, double value) {
    enqueueRequest(mDeviceProtocol->createRequestSetVoltage(channel, value));
}

void Communication::getVoltage(TChannel channel) {
    enqueueRequest(mDeviceProtocol->createRequestGetVoltage(channel));
}

void Communication::getOutputCurrent(TChannel channel) {
    enqueueRequest(mDeviceProtocol->createRequestGetOutputCurrent(channel));
}

void Communication::getOutputVoltage(TChannel channel) {
    enqueueRequest(mDeviceProtocol->createRequestGetOutputVoltage(channel));
}

void Communication::setEnableOutputSwitch(bool enable) {
    enqueueRequest(mDeviceProtocol->createRequestEnableOutputSwitch(enable));
}

void Communication::enableBuzzer(bool enable) {
    enqueueRequest(mDeviceProtocol->createRequestEnableBuzzer(enable));
}

void Communication::isBuzzerEnabled() {
    enqueueRequest(mDeviceProtocol->createRequestIsBuzzerEnabled());
}

void Communication::getDeviceStatus() {
    enqueueRequest(mDeviceProtocol->createRequestGetDeviceStatus());
}

void Communication::getDeviceID() {
    enqueueRequest(mDeviceProtocol->createRequestGetDeviceID());
}

void Communication::applySettings(TMemoryKey key) {
    enqueueRequest(mDeviceProtocol->createRequestApplySettings(key));
}

void Communication::getActiveSettings() {
    enqueueRequest(mDeviceProtocol->createRequestGetActiveSettings());
}

void Communication::saveSettings(TMemoryKey key) {
    enqueueRequest(mDeviceProtocol->createRequestSaveSettings(key));
}

void Communication::changeOutputConnectionMethod(TOutputConnectionMethod method) {
    enqueueRequest(mDeviceProtocol->createRequestChangeOutputConnectionMethod(method));
}

void Communication::enableOverCurrentProtection(bool enable) {
    enqueueRequest(mDeviceProtocol->createRequestEnableOverCurrentProtection(enable));
}

void Communication::enableOverVoltageProtection(bool enable) {
    enqueueRequest(mDeviceProtocol->createRequestEnableOverVoltageProtection(enable));
}

void Communication::setOverCurrentProtectionValue(TChannel channel, double current) {
    enqueueRequest(mDeviceProtocol->createRequestSetOverCurrentProtectionValue(channel, current));
}

void Communication::getOverCurrentProtectionValue(TChannel channel) {
    enqueueRequest(mDeviceProtocol->createRequestGetOverCurrentProtectionValue(channel));
}

void Communication::setOverVoltageProtectionValue(TChannel channel, double voltage) {
    enqueueRequest(mDeviceProtocol->createRequestSetOverVoltageProtectionValue(channel, voltage));
}

void Communication::getOverVoltageProtectionValue(TChannel channel) {
    enqueueRequest(mDeviceProtocol->createRequestGetOverVoltageProtectionValue(channel));
}





