//
// Created by Vitalii Arkusha on 12.09.2022.
//

#include "Communication.h"
#include <QTimer>
#include <QDebug>
#include <QThread>

#define DELAY_BETWEEN_REQUEST_MS 40

Communication::Communication(QObject *parent) : QObject(parent){
    mSerialPort.setDataBits(QSerialPort::Data8);
    mSerialPort.setParity(QSerialPort::NoParity);
    mSerialPort.setStopBits(QSerialPort::OneStop);
    mSerialPort.setFlowControl(QSerialPort::NoFlowControl);


    mRequestQueueTimer.setTimerType(Qt::PreciseTimer);
    mRequestQueueTimer.setInterval(1);

    connect(&mSerialPort, &QSerialPort::readyRead, this, &Communication::slotSerialReadData);
    connect(&mRequestQueueTimer, &QTimer::timeout, this, &Communication::slotProcessRequestQueue);
}

void Communication::openSerialPort(const QString &name, int baudRate) {
    closeSerialPort();

    mSerialPort.setPortName(name);
    mSerialPort.setBaudRate(baudRate);

    if (mSerialPort.open(QIODevice::ReadWrite)) {
        mSerialPort.clear();
        mRequestQueueTimer.start();
        emit onSerialPortOpened();
    } else {
        emit onSerialPortError(mSerialPort.errorString());
    }
}

void Communication::closeSerialPort() {
    if (mSerialPort.isOpen()) {
        mSerialPort.close();
        mRequestQueueTimer.stop();
        emit onSerialPortClosed();
    }
}

QStringList Communication::availableSerialPorts() {
    QStringList serialPorts;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        serialPorts << info.portName();
    }

    return serialPorts;
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
    if (mSerialPort.bytesAvailable() >= request->responseDataSize()) {
        QByteArray buff(mSerialPort.read(request->responseDataSize()));
        dispatchData(*request, buff);

        delete request;
        mRequestQueue.dequeue();

        serialSendRequest(true); // here we are able to send request (process queue) immediately
    }
}

void Communication::dispatchData(const Protocol::IProtocol &request, const QByteArray &data) {
    if (typeid(request) == typeid(Protocol::GetOutputStatus)) {
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
    } else if (typeid(request) == typeid(Protocol::GetDeviceInfo)) {
        emit onDeviceInfo(data);
    } else if (typeid(request) == typeid(Protocol::GetRecalledSetting)) {
        emit onRecalledSetting(TMemoryKey(data.toInt()));
    } else if (typeid(request) == typeid(Protocol::IsOperationPanelLocked)) {
        emit onOperationPanelLocked(bool(data.toInt()));
    } else if (typeid(request) == typeid(Protocol::IsBuzzerEnabled)) {
        emit onBeepEnabled(bool(data.toInt()));
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
    mSerialPort.write(request->requestQuery());
    mRequestNextTime = QTime::currentTime().addMSecs(DELAY_BETWEEN_REQUEST_MS);

    mSerialPort.flush();

    if (request->responseDataSize() == 0) { // not expect response, remove
        mRequestQueue.dequeue();
    }
}

void Communication::enqueueRequest(Protocol::IProtocol *request) {
    mRequestQueue.enqueue(request);
}

void Communication::lockOperationPanel(bool lock) {
    enqueueRequest(new Protocol::LockOperationPanel(lock));
}

void Communication::isOperationPanelLocked() {
    enqueueRequest(new Protocol::IsOperationPanelLocked());
}

void Communication::setCurrent(TChannel channel, double value) {
    enqueueRequest(new Protocol::SetCurrent(channel, value));
}

void Communication::getCurrent(TChannel channel) {
    enqueueRequest(new Protocol::GetCurrent(channel));
}

void Communication::setVoltage(TChannel channel, double value) {
    enqueueRequest(new Protocol::SetVoltage(channel, value));
}

void Communication::getVoltage(TChannel channel) {
    enqueueRequest(new Protocol::GetVoltage(channel));
}

void Communication::getOutputCurrent(TChannel channel) {
    enqueueRequest(new Protocol::GetOutputCurrent(channel));
}

void Communication::getOutputVoltage(TChannel channel) {
    enqueueRequest(new Protocol::GetOutputVoltage(channel));
}

void Communication::setOutputSwitch(bool ON) {
    enqueueRequest(new Protocol::SetOutputSwitch(ON));
}

void Communication::enableBeep(bool enable) {
    enqueueRequest(new Protocol::EnableBuzzer(enable));
}

void Communication::isBeepEnabled() {
    enqueueRequest(new Protocol::IsBuzzerEnabled());
}

void Communication::getDeviceStatus() {
    enqueueRequest(new Protocol::GetOutputStatus());
}

void Communication::getDeviceInfo() {
    enqueueRequest(new Protocol::GetDeviceInfo());
}

void Communication::recallSetting(TMemoryKey key) {
    enqueueRequest(new Protocol::RecallSetting(key));
}

void Communication::getActiveSetting() {
    enqueueRequest(new Protocol::GetRecalledSetting());
}

void Communication::saveSetting(TMemoryKey key) {
    enqueueRequest(new Protocol::SaveSetting(key));
}

void Communication::changeOutputConnectionMethod(TOutputConnectionMethod method) {
    enqueueRequest(new Protocol::OutputConnectionMethod(method));
}

void Communication::enableOverCurrentProtection(bool enable) {
    enqueueRequest(new Protocol::EnableOverCurrentProtection(enable));
}

void Communication::enableOverVoltageProtection(bool enable) {
    enqueueRequest(new Protocol::EnableOverVoltageProtection(enable));
}

void Communication::setOverCurrentProtectionValue(TChannel channel, double current) {
    enqueueRequest(new Protocol::SetOverCurrentProtectionValue(channel, current));
}

void Communication::getOverCurrentProtectionValue(TChannel channel) {
    enqueueRequest(new Protocol::GetOverCurrentProtectionValue(channel));
}

void Communication::setOverVoltageProtectionValue(TChannel channel, double voltage) {
    enqueueRequest(new Protocol::SetOverVoltageProtectionValue(channel, voltage));
}

void Communication::getOverVoltageProtectionValue(TChannel channel) {
    enqueueRequest(new Protocol::GetOverVoltageProtectionValue(channel));
}




