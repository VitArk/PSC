//
// Created by Vitalii Arkusha on 14.10.2022.
//

#ifndef PSM_UTP3305C_H
#define PSM_UTP3305C_H

#include "Device.h"
namespace Protocol {
    class UTP3305C : public Device {
    public:
        QString name() const override {
            return "UNI-T UTP3305C";
        }

        QString id() const override {
            return "P3305C%**";
        };

        QString description() const override {
            return QString("%1 Programmable DC Power Supply").arg(name());
        }

        double maxChannelCurrent() const override {
            return 5.100; // Amp
        };

        double maxChannelVoltage() const override {
            return 31.00; // Volts
        };
    };
}
#endif //PSM_UTP3305C_H
