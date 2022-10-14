//
// Created by Vitalii Arkusha on 28.09.2022.
//

#ifndef PSC_DEVICESTATUS_H
#define PSC_DEVICESTATUS_H

#include <QObject>
#include <QDebug>
#include "Commons.h"

class DeviceStatus {
public:
    DeviceStatus(char data);
    TOutputStabilizingMode stabilizingMode(TChannel channel) const;
    TOutputConnectionMethod outputConnectionMethod() const;
    TOutputProtection outputProtectionMode() const;
    bool outputSwitchStatus() const;

    QByteArray data() const;
private:
    char mData;
};

QDebug operator<<(QDebug debug, const DeviceStatus &c);

#endif //PSC_DEVICESTATUS_H
