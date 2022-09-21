//
// Created by Vitalii Arkusha on 12.09.2022.
//

#include "Communication.h"
#include <QTimer>
#include <QDebug>
#include <QThread>

#define DELAY_BETWEEN_REQUEST_MS 30

Communication::Communication(QObject *parent) : QObject(parent){
    mSerialPort.setDataBits(QSerialPort::Data8);
    mSerialPort.setParity(QSerialPort::NoParity);
    mSerialPort.setStopBits(QSerialPort::OneStop);
    mSerialPort.setFlowControl(QSerialPort::NoFlowControl);

    connect(&mSerialPort, &QSerialPort::readyRead, this, &Communication::serialReadData);

}

void Communication::openSerialPort(const QString &name, int baudRate) {
    closeSerialPort();

    mSerialPort.setPortName(name);
    mSerialPort.setBaudRate(baudRate);

    if (mSerialPort.open(QIODevice::ReadWrite)) {
        emit onSerialPortOpened();
    } else {
        emit onSerialPortError(mSerialPort.errorString());
    }
}

void Communication::closeSerialPort() {
    if (mSerialPort.isOpen()) {
        mSerialPort.close();
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

void Communication::serialReadData() {
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

        serialSendRequest(); // here we are able to send request (process queue) immediately
    }
}

void Communication::dispatchData(const Protocol::IProtocol &request, const QByteArray &data) {
    if (typeid(request) == typeid(Protocol::ReadOutputStatus)) {
        char d = data.at(0);
        emit onOutputStatus(TOutputStatus(d & 0x1), TOutputStatus(d & 0x2), d & 0x40);
    } else if (typeid(request) == typeid(Protocol::ReadOutputCurrent)) {
        emit onOutputCurrent(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::ReadOutputVoltage)) {
        emit onOutputVoltage(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::ReadSetCurrent)) {
        emit onSetCurrent(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::ReadSetVoltage)) {
        emit onSetVoltage(request.channel(), data.toDouble());
    } else if (typeid(request) == typeid(Protocol::ReadDeviceInfo)) {
        emit onDeviceInfo(data);
    } else if (typeid(request) == typeid(Protocol::ReadActiveSetting)) {
        emit onActiveSetting(TMemoryKey(data.toInt()));
    }
}

void Communication::serialSendRequest() {
    if (mRequestQueue.isEmpty()) {
        return;
    }

    auto request = mRequestQueue.head();
    mSerialPort.write(request->requestQuery());
    mSerialPort.flush();

    if (request->responseDataSize() == 0) { // not expect response
        mRequestQueue.dequeue();
        // we need to give some time for executing the request on the device.
        QTimer::singleShot(DELAY_BETWEEN_REQUEST_MS, this, &Communication::serialSendRequest);
    }
}

void Communication::enqueueRequest(Protocol::IProtocol *request) {
    mRequestQueue.enqueue(request);
    if (mRequestQueue.length() == 1) { // just added request, and that means no executing requests at the moment.
        // We need to give some time for executing the request on the device.
        // So we have to take care about delay between request.
        QTimer::singleShot(DELAY_BETWEEN_REQUEST_MS, this, &Communication::serialSendRequest);
    }
}

void Communication::LockOperationPanel(bool state) {
    enqueueRequest(new Protocol::Lock(state));
}

void Communication::SetCurrent(TChannel channel, double value) {
    enqueueRequest(new Protocol::SetCurrent(channel, value));
}

void Communication::ReadSetCurrent(TChannel channel) {
    enqueueRequest(new Protocol::ReadSetCurrent(channel));
}

void Communication::SetVoltage(TChannel channel, double value) {
    enqueueRequest(new Protocol::SetVoltage(channel, value));
}

void Communication::ReadSetVoltage(TChannel channel) {
    enqueueRequest(new Protocol::ReadSetVoltage(channel));
}

void Communication::ReadOutputCurrent(TChannel channel) {
    enqueueRequest(new Protocol::ReadOutputCurrent(channel));
}

void Communication::ReadOutputVoltage(TChannel channel) {
    enqueueRequest(new Protocol::ReadOutputVoltage(channel));
}

void Communication::Output(bool state) {
    enqueueRequest(new Protocol::Output(state));
}

void Communication::Buzzer(bool state) {
    enqueueRequest(new Protocol::Buzzer(state));
}

void Communication::ReadOutputStatus() {
    enqueueRequest(new Protocol::ReadOutputStatus());
}

void Communication::ReadDeviceInfo() {
    enqueueRequest(new Protocol::ReadDeviceInfo());
}

void Communication::RecallSetting(TMemoryKey key) {
    enqueueRequest(new Protocol::RecallSetting(key));
}

void Communication::ReadActiveSetting() {
    enqueueRequest(new Protocol::ReadActiveSetting());
}

void Communication::SaveSetting(TMemoryKey key) {
    enqueueRequest(new Protocol::SaveSetting(key));
}

void Communication::OutputMode(TOutputMode mode) {
    enqueueRequest(new Protocol::OutputMode(mode));
}

void Communication::OverCurrentProtection(bool state) {
    enqueueRequest(new Protocol::OverCurrentProtection(state));
}

void Communication::OverVoltageProtection(bool state) {
    enqueueRequest(new Protocol::OverVoltageProtection(state));
}

void Communication::SetOverCurrentProtection(TChannel channel, double value) {
    enqueueRequest(new Protocol::SetOverCurrentProtection(channel, value));
}

void Communication::SetOverVoltageProtection(TChannel channel, double value) {
    enqueueRequest(new Protocol::SetOverVoltageProtection(channel, value));
}
