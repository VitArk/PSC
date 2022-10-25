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

#ifndef PSM_UTP3303C_H
#define PSM_UTP3303C_H

#include "Interface.h"
namespace Protocol {
    class UTP3303C : public Interface {
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

        int activeChannels() const override {
            return 2;
        }
    };
}

#endif //PSM_UTP3303C_H
