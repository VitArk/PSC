//
// Created by Vitalii Arkusha on 14.10.2022.
//
#include "Factory.h"
#include <QDebug>
#include <QScopedPointer>
namespace Protocol {

    Factory::Factory(QSerialPort &serialPort) : mSerialPort(serialPort) {
        mSerialPort.blockSignals(true);
        mIdRequestQueries << "*IDN?";
    }

    Device *Factory::create(const QByteArray &deviceID) {
        Device *device = nullptr;
        if (UTP3305C().checkID(deviceID)) {
            return new UTP3305C();
        }
        if (UTP3303C().checkID(deviceID)) {
            return new UTP3303C();
        }

        return device;
    }

    Factory::~Factory() {
        mSerialPort.blockSignals(false);
    }

    Device *Factory::create() {
        if (!mSerialPort.isOpen()) {
            return nullptr;
        }

        QByteArray responseData;
        foreach (const QByteArray &query, mIdRequestQueries) {
            mSerialPort.write(query);
            if (!mSerialPort.waitForBytesWritten(200)) {
                mErrorString = QObject::tr("Wait write request timeout");
                return nullptr;
            }

            if (mSerialPort.waitForReadyRead(200)) {
                responseData = mSerialPort.readAll();
                while (mSerialPort.waitForReadyRead(10)) {
                    responseData += mSerialPort.readAll();
                }

                if (responseData.isEmpty()) {
                    continue;
                }

                auto pProtocol = create(responseData);
                if (pProtocol == nullptr) {
                    break;
                }
                return pProtocol;
            } else {
                mErrorString = QObject::tr("Wait read response timeout");
            }
        }

        mErrorString = responseData;
        return nullptr;
    }

    QString Factory::errorString() const {
        return mErrorString;
    }

}