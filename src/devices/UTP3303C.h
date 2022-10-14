//
// Created by Vitalii Arkusha on 14.10.2022.
//

#ifndef PSM_UTP3303C_H
#define PSM_UTP3303C_H

#include "Device.h"
namespace Protocol {
    class UTP3303C : public Device {
    public:
        QString name() const override {
            return "UNI-T UTP3303C";
        }

        QString id() const override {
            return "P3303C%**";
        };

        QString description() const override {
            return QString("%1 Programmable DC Power Supply").arg(name());
        }

        double maxChannelCurrent() const override {
            return 3.000; // Amp
        }

        double maxChannelVoltage() const override {
            return 30.00; // Volts
        }
    };
}

#endif //PSM_UTP3303C_H
