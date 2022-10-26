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

#ifndef PSM_BASESCPI_H
#define PSM_BASESCPI_H

#include <QString>
#include <QScopedPointer>
#include "Commons.h"
#include "Messages.h"

namespace Protocol {
class BaseSCPI {
public:
    virtual ~BaseSCPI() = default;

    virtual QString name() const = 0;
    virtual QString deviceID() const = 0;
    virtual QString description() const = 0;
    virtual double maxChannelCurrent() const = 0;
    virtual double maxChannelVoltage() const = 0;
    virtual int activeChannelsCount() const = 0;

    virtual bool isRecognized(QString deviceID) const {
        return deviceID == this->deviceID();
    }

    virtual DeviceInfo deviceInfo() const {
        return {
            .name = name(),
            .ID = deviceID(),
            .description = description(),
            .maxChannelCurrent = maxChannelCurrent(),
            .maxChannelVoltage = maxChannelVoltage(),
            .activeChannelsCount = activeChannelsCount(),
        };
    }

    virtual DeviceStatus processDeviceStatusReply(const QByteArray &rawReply) const {
        Q_ASSERT(!rawReply.isEmpty());
        return {
            .channel1Mode   = evaluateOutputMode(rawReply, Channel1),
            .channel2Mode   = evaluateOutputMode(rawReply, Channel2),
            .tracking       = evaluateChannelTracking(rawReply),
            .protection     = evaluateOutputProtection(rawReply),
            .outputSwitch   = evaluateOutputSwitchState(rawReply),
        };
    }

    virtual IMessage* createMessageSetLocked(bool lock) {
        return new MessageSetLocked(lock);
    }
    virtual IMessage* createMessageGetIsLocked() {
        return new MessageGetIsLocked();
    }
    virtual IMessage* createMessageSetCurrent(Channel channel, double value) {
        return new MessageSetCurrent(channel, value);
    }
    virtual IMessage* createMessageGetCurrentSet(Channel channel) {
        return new MessageGetCurrentSet(channel);
    }
    virtual IMessage* createMessageSetVoltage(Channel channel, double value) {
        return new MessageSetVoltage(channel, value);
    }
    virtual IMessage* createMessageGetVoltageSet(Channel channel) {
        return new MessageGetVoltageSet(channel);
    }
    virtual IMessage* createMessageGetActualCurrent(Channel channel) {
        return new MessageGetActualCurrent(channel);
    }
    virtual IMessage* createMessageGetActualVoltage(Channel channel) {
        return new MessageGetActualVoltage(channel);
    }
    virtual IMessage* createMessageSetEnableOutputSwitch(bool enable) {
        return new MessageSetEnableOutputSwitch(enable);
    }
    virtual IMessage* createMessageSetEnableBeep(bool enable) {
        return new MessageSetEnableBeep(enable);
    }
    virtual IMessage* createMessageGetIsBeepEnabled() {
        return new MessageGetIsBeepEnabled();
    }
    virtual IMessage* createMessageGetDeviceStatus() {
        return new MessageGetDeviceStatus();
    }
    virtual IMessage* createMessageGetDeviceID() {
        return new MessageGetDeviceID();
    }
    virtual IMessage* createMessageSetPreset(MemoryKey key) {
        return new MessageSetPreset(key);
    }
    virtual IMessage* createMessageGetPreset() {
        return new MessageGetPreset();
    }
    virtual IMessage* createMessageSavePreset(MemoryKey key) {
        return new MessageSavePreset(key);
    }
    virtual IMessage* createMessageSetChannelTracking(ChannelTracking method) {
        return new MessageSetChannelTracking(method);
    }
    virtual IMessage* createMessageSetEnableOverCurrentProtection(bool enable) {
        return new MessageSetEnableOverCurrentProtection(enable);
    }
    virtual IMessage* createMessageSetEnableOverVoltageProtection(bool enable) {
        return new MessageSetEnableOverVoltageProtection(enable);
    }
    virtual IMessage* createMessageSetOverCurrentProtectionValue(Channel channel, double current) {
        return new MessageSetOverCurrentProtectionValue(channel, current);
    }
    virtual IMessage* createMessageGetOverCurrentProtectionValue(Channel channel) {
        return new MessageGetOverCurrentProtectionValue(channel);
    }
    virtual IMessage* createMessageSetOverVoltageProtectionValue(Channel channel, double voltage) {
        return new MessageSetOverVoltageProtectionValue(channel, voltage);
    }
    virtual IMessage* createMessageGetOverVoltageProtectionValue(Channel channel) {
        return new MessageGetOverVoltageProtectionValue(channel);
    }

protected:
    virtual OutputMode evaluateOutputMode(const QByteArray &data, Channel channel) const {
        return channel == Channel1 ? OutputMode(data[0] & 0x1) : OutputMode(data[0] & 0x2);
    }

    virtual ChannelTracking evaluateChannelTracking(const QByteArray &data) const {
        ChannelTracking tracking = Independent;
        tracking = bool(data[0] & 0x4) ? Serial : tracking;
        tracking = bool(data[0] & 0x8) ? Parallel : tracking;

        return tracking;
    }

    virtual OutputProtection evaluateOutputProtection(const QByteArray &data) const {
        OutputProtection protection = OutputProtectionAllDisabled;
        protection = bool(data[0] & 0x10) ? OverVoltageProtectionOnly : protection;
        protection = bool(data[0] & 0x20) ? OverCurrentProtectionOnly : protection;
        protection = bool((data[0] & 0x30) == 0x30) ? OutputProtectionAllEnabled : protection;

        return protection;
    }

    virtual bool evaluateOutputSwitchState(const QByteArray &data) const {
        return bool(data[0] & 0x40);
    }
};
}

#endif //PSM_BASESCPI_H
