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
// Created by Vitalii Arkusha on 13.05.2021.
//

#ifndef POWER_SUPPLY_CONTROLLER_GLOBAL_H
#define POWER_SUPPLY_CONTROLLER_GLOBAL_H

#include <QString>

namespace Global {
    enum Channel {
        Channel1 = 1,
        Channel2 = 2,
    };

    enum MemoryKey {
        M1 = 1,
        M2 = 2,
        M3 = 3,
        M4 = 4,
        M5 = 5,
    };

    enum ChannelsTracking {
        Independent = 0,
        Serial = 1,
        Parallel = 2,
    };

    enum OutputMode {
        ConstantCurrent = false,
        ConstantVoltage = true,
    };

    enum OutputProtection {
        OutputProtectionAllDisabled = 0,
        OutputProtectionAllEnabled = 1,
        OverVoltageProtectionOnly = 2,
        OverCurrentProtectionOnly = 3,
    };

    struct DeviceStatus {
        OutputMode         ModeCh1;
        OutputMode         ModeCh2;
        ChannelsTracking    Tracking;
        OutputProtection   Protection;
        bool               OutputSwitch;
    };
    
    struct DeviceInfo {
        QString Name;
        QString ID;
        QString Description;

        double MinCurrent;
        double MaxCurrent; // Amperes
        double CurrentSetPrecision;

        double MinVoltage;
        double MaxVoltage; // Volts
        double VoltageSetPrecision;

        int ActiveChannelsCount;       // Only active channels, ignore fixed.
    };
}

#endif //POWER_SUPPLY_CONTROLLER_GLOBAL_H
