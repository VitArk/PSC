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

struct DeviceInfo{
    QString name;
    QString ID;
    QString description;
    double maxChannelCurrent; // Amperes
    double maxChannelVoltage; // Volts
    int activeChannels;       // Only active channels, ignore fixed.
};

enum Channel {
    Channel1 = 1,
    Channel2 = 2,
} typedef TChannel;

enum MemoryKey {
    M1 = 1,
    M2 = 2,
    M3 = 3,
    M4 = 4,
    M5 = 5,
} typedef TMemoryKey;

enum ChannelTracking {
    Independent = 0,
    Serial = 1,
    Parallel = 2,
} typedef TChannelTracking;

enum OutputMode {
    ConstantCurrent = 0,
    ConstantVoltage = 1,
} typedef TOutputMode;

enum OutputProtection {
    OutputProtectionAllDisabled = 0,
    OutputProtectionAllEnabled = 1,
    OverVoltageProtectionOnly = 2,
    OverCurrentProtectionOnly = 3,

} typedef TOutputProtection;


#endif //POWER_SUPPLY_CONTROLLER_COMMONS_H
