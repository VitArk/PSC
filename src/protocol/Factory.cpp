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
// Created by Vitalii Arkusha on 14.10.2022.
//
#include "Factory.h"

namespace Protocol {
    Factory::Factory(QSerialPort &serialPort) : mSerialPort(serialPort) {
        mSerialPort.blockSignals(true);
        mKnownGetIDQueryList << "*IDN?";
    }

    Interface *Factory::create() {
        return createInstance(deviceIdentification());
    }

    Interface *Factory::createInstance(const QByteArray &deviceID) {
        if (UTP3305C().checkID(deviceID)) {
            return new UTP3305C();
        }
        if (UTP3303C().checkID(deviceID)) {
            return new UTP3303C();
        }

        mErrorString = deviceID;
        return nullptr;
    }

    Factory::~Factory() {
        mSerialPort.blockSignals(false);
    }

    QByteArray Factory::deviceIdentification() {
        if (!mSerialPort.isOpen()) {
            return nullptr;
        }

        foreach (const QByteArray &query, mKnownGetIDQueryList) {
            mSerialPort.write(query);
            if (!mSerialPort.waitForBytesWritten(200)) {
                mErrorString = QObject::tr("Wait write identification command timeout");
                return nullptr;
            }

            if (mSerialPort.waitForReadyRead(200)) {
                QByteArray replyData = mSerialPort.readAll();
                while (mSerialPort.waitForReadyRead(10)) {
                    replyData += mSerialPort.readAll();
                }

                if (replyData.isEmpty()) {
                    continue;
                }

                return replyData;
            } else {
                mErrorString = QObject::tr("Wait for read for identification command timeout");
            }
        }

        return "";
    }

    QString Factory::errorString() const {
        return mErrorString;
    }



}