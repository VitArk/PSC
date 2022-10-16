//
// Created by Vitalii Arkusha on 14.10.2022.
//

#ifndef PSM_FACTORY_H
#define PSM_FACTORY_H

#include "Device.h"
#include "UTP3303C.h"
#include "UTP3305C.h"

namespace Protocol {
    class Factory {
    public:
        static Device *create(const QString& deviceID) {
            Device *device = nullptr;
            qDebug() << deviceID;
            if (UTP3305C().checkID(deviceID)) {
                return new UTP3305C();
            }
            if (UTP3303C().checkID(deviceID)) {
                return new UTP3303C();
            }

            return device;
        }
    };
}

#endif //PSM_FACTORY_H
