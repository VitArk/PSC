//
// Created by Vitalii Arkusha on 14.10.2022.
//

#ifndef PSM_FACTORY_H
#define PSM_FACTORY_H

#include <QByteArrayList>
#include <QSerialPort>

#include "Device.h"
#include "UTP3303C.h"
#include "UTP3305C.h"

namespace Protocol {
    class Factory  {
    public:
        explicit Factory(QSerialPort &serialPort);
        ~Factory();
        Device *create();

        QString errorString() const;

    private:
        QSerialPort &mSerialPort;
        QByteArrayList mIdRequestQueries;
        QString        mErrorString;

    private:
        static Device* create(const QByteArray& deviceID);
    };
}

#endif //PSM_FACTORY_H
