//
// Created by Vitalii Arkusha on 13.05.2021.
//

#ifndef POWER_SUPPLY_CONTROLLER_COMMONS_H
#define POWER_SUPPLY_CONTROLLER_COMMONS_H

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

enum OutputMode {
    Independent = 1,
    Serial = 2,
    Parallel = 3,
};

#endif //POWER_SUPPLY_CONTROLLER_COMMONS_H
