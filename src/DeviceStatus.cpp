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
// Created by Vitalii Arkusha on 28.09.2022.
//

#include "DeviceStatus.h"

const char maskOutModeCh1 = 0x1;
const char maskOutModeCh2 = 0x2;
const char maskSerialOutputMethod = 0x4;
const char maskSerialParallelMethod = 0x8;
const char maskOverVoltageProtection = 0x10;
const char maskOverCurrentProtection = 0x20;
const char maskOverProtectionAll = 0x30;
const char maskOutputSwitch = 0x40;

DeviceStatus::DeviceStatus(char data) {
    mData = data;
}

TOutputMode DeviceStatus::mode(TChannel channel) const {
    if (channel == Channel1)
        return TOutputMode(mData & maskOutModeCh1);
    else
        return TOutputMode(mData & maskOutModeCh2);
}

TChannelTracking DeviceStatus::tracking() const {
    TChannelTracking connMethod = Independent;
    connMethod = bool(mData & maskSerialOutputMethod) ? Serial : connMethod;
    connMethod = bool(mData & maskSerialParallelMethod) ? Parallel : connMethod;

    return connMethod;
}

TOutputProtection DeviceStatus::protection() const {
    TOutputProtection protection = OutputProtectionAllDisabled;
    protection = bool(mData & maskOverVoltageProtection) ? OverVoltageProtectionOnly : protection;
    protection = bool(mData & maskOverCurrentProtection) ? OverCurrentProtectionOnly : protection;
    protection = bool((mData & maskOverProtectionAll) == maskOverProtectionAll) ? OutputProtectionAllEnabled : protection;

    return protection;
}

bool DeviceStatus::outputSwitch() const {
    return bool(mData & maskOutputSwitch);
}

QByteArray DeviceStatus::data() const {
    return {1, mData};
}

QDebug operator<<(QDebug debug, const DeviceStatus &c) {
    QDebugStateSaver saver(debug);
    debug.nospace() << c.data().toHex();

    return debug;
}
