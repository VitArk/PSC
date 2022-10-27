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

#ifndef POWER_SUPPLY_CONTROLLER_COMMONS_H
#define POWER_SUPPLY_CONTROLLER_COMMONS_H

namespace Protocol {
    struct DeviceInfo {
        QString Name;
        QString ID;
        QString Description;
        double MaxCurrent; // Amperes
        double MaxVoltage; // Volts
        int ActiveChannelsCount;       // Only active channels, ignore fixed.
    };

    enum Channel {
        Channel1 = 1,
        Channel2 = 2,
    };

    enum MemoryKey {
        Memory1 = 1,
        Memory2 = 2,
        Memory3 = 3,
        Memory4 = 4,
        Memory5 = 5,
    };

    enum ChannelTracking {
        Independent = 0,
        Serial = 1,
        Parallel = 2,
    };

    enum OutputMode {
        ConstantCurrent = 0,
        ConstantVoltage = 1,
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
        ChannelTracking    Tracking;
        OutputProtection   Protection;
        bool               OutputSwitch;
    };

}

#endif //POWER_SUPPLY_CONTROLLER_COMMONS_H
