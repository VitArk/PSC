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

TOutputStabilizingMode DeviceStatus::stabilizingMode(TChannel channel) const {
    if (channel == Channel1)
        return TOutputStabilizingMode(mData & maskOutModeCh1);
    else
        return TOutputStabilizingMode(mData & maskOutModeCh2);
}

TOutputConnectionMethod DeviceStatus::outputConnectionMethod() const {
    TOutputConnectionMethod connMethod = Independent;
    connMethod = bool(mData & maskSerialOutputMethod) ? Serial : connMethod;
    connMethod = bool(mData & maskSerialParallelMethod) ? Parallel : connMethod;

    return connMethod;
}

TOutputProtection DeviceStatus::outputProtectionMode() const {
    TOutputProtection protection = OutputProtectionAllDisabled;
    protection = bool(mData & maskOverVoltageProtection) ? OverVoltageProtectionOnly : protection;
    protection = bool(mData & maskOverCurrentProtection) ? OverCurrentProtectionOnly : protection;
    protection = bool((mData & maskOverProtectionAll) == maskOverProtectionAll) ? OutputProtectionAllEnabled : protection;

    return protection;
}

bool DeviceStatus::outputSwitchStatus() const {
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
