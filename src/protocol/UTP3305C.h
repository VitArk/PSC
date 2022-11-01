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

#ifndef PSM_UTP3305C_H
#define PSM_UTP3305C_H

#include "BaseSCPI.h"
namespace Protocol {
    class UTP3305C : public BaseSCPI {
    public:
        QString name() const override {
            return "UNI-T UTP3305C";
        }

        QString deviceID() const override {
            return "P3305C%**";
        };

        QString description() const override {
            return QString("%1 Programmable DC Power Supply").arg(name());
        }

        double minChannelCurrent() const override {
            return 0.000; // Amp
        };

        double maxChannelCurrent() const override {
            return 5.100; // Amp
        };

        double currentSetPrecision() const override {
            return 0.001;
        };

        double minChannelVoltage() const override {
            return 0.00; // Volts
        };

        double maxChannelVoltage() const override {
            return 31.00; // Volts
        };

        double voltageSetPrecision() const override {
            return 0.01;
        }

        int activeChannelsCount() const override {
            return 2;
        }
    };
}
#endif //PSM_UTP3305C_H
