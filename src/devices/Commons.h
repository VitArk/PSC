//
// Created by Vitalii Arkusha on 13.05.2021.
//

#ifndef POWER_SUPPLY_CONTROLLER_COMMONS_H
#define POWER_SUPPLY_CONTROLLER_COMMONS_H

struct DeviceInfo{
    QString name;
    QString ID;
    QString description;
    double maxChannelCurrent;
    double maxChannelVoltage;
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

enum OutputConnectionMethod {
    Independent = 0,
    Serial = 1,
    Parallel = 2,
} typedef TOutputConnectionMethod;

enum OutputStabilizingMode {
    ConstantCurrent = 0,
    ConstantVoltage = 1,
} typedef TOutputStabilizingMode;

enum OutputProtection {
    OutputProtectionAllDisabled = 0,
    OutputProtectionAllEnabled = 1,
    OverVoltageProtectionOnly = 2,
    OverCurrentProtectionOnly = 3,

} typedef TOutputProtection;


#endif //POWER_SUPPLY_CONTROLLER_COMMONS_H
