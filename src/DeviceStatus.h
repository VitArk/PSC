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

#ifndef PSC_DEVICESTATUS_H
#define PSC_DEVICESTATUS_H

#include <QObject>
#include <QDebug>
#include "protocol/Commons.h"

class DeviceStatus {
public:
    DeviceStatus(char data);
    TOutputMode mode(TChannel channel) const;
    TChannelTracking tracking() const;
    TOutputProtection protection() const;
    bool outputSwitch() const;

    QByteArray data() const;
private:
    char mData;
};

QDebug operator<<(QDebug debug, const DeviceStatus &c);

#endif //PSC_DEVICESTATUS_H
