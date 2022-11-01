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
#include <QDebug>
#include "Global.h"
#include "Messages.h"


namespace Protocol {
class BaseSCPI {
public:
    virtual ~BaseSCPI() = default;

    virtual QString name() const = 0;
    virtual QString deviceID() const = 0;
    virtual QString description() const = 0;

    virtual double minChannelCurrent() const = 0;
    virtual double maxChannelCurrent() const = 0;
    virtual double currentSetPrecision() const = 0;

    virtual double minChannelVoltage() const = 0;
    virtual double maxChannelVoltage() const = 0;
    virtual double voltageSetPrecision() const = 0;
    virtual int activeChannelsCount() const = 0;

    virtual bool isRecognized(QString deviceID) const {
        return deviceID == this->deviceID();
    }

    virtual Global::DeviceInfo deviceInfo() const {
        return {
            .Name = name(),
            .ID = deviceID(),
            .Description = description(),
            .MinCurrent = minChannelCurrent(),
            .MaxCurrent = maxChannelCurrent(),
            .CurrentSetPrecision = currentSetPrecision(),
            .MinVoltage = minChannelVoltage(),
            .MaxVoltage = maxChannelVoltage(),
            .VoltageSetPrecision = voltageSetPrecision(),
            .ActiveChannelsCount = activeChannelsCount(),
        };
    }

    virtual Global::DeviceStatus processDeviceStatusReply(const QByteArray &rawReply) const {
        Q_ASSERT(!rawReply.isEmpty());
        return {
            .ModeCh1        = evaluateOutputMode(rawReply, Global::Channel1),
            .ModeCh2        = evaluateOutputMode(rawReply, Global::Channel2),
            .Tracking       = evaluateChannelTracking(rawReply),
            .Protection     = evaluateOutputProtection(rawReply),
            .OutputSwitch   = evaluateOutputSwitchState(rawReply),
        };
    }

    virtual IMessage* createMessageSetLocked(bool lock) {
        return new MessageSetLocked(lock);
    }
    virtual IMessage* createMessageGetIsLocked() {
        return new MessageGetIsLocked();
    }
    virtual IMessage* createMessageSetCurrent(Global::Channel channel, double value) {
        return new MessageSetCurrent(channel, value);
    }
    virtual IMessage* createMessageGetCurrentSet(Global::Channel channel) {
        return new MessageGetCurrentSet(channel);
    }
    virtual IMessage* createMessageSetVoltage(Global::Channel channel, double value) {
        return new MessageSetVoltage(channel, value);
    }
    virtual IMessage* createMessageGetVoltageSet(Global::Channel channel) {
        return new MessageGetVoltageSet(channel);
    }
    virtual IMessage* createMessageGetActualCurrent(Global::Channel channel) {
        return new MessageGetActualCurrent(channel);
    }
    virtual IMessage* createMessageGetActualVoltage(Global::Channel channel) {
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
    virtual IMessage* createMessageSetPreset(Global::MemoryKey key) {
        return new MessageSetPreset(key);
    }
    virtual IMessage* createMessageGetPreset() {
        return new MessageGetPreset();
    }
    virtual IMessage* createMessageSavePreset(Global::MemoryKey key) {
        return new MessageSavePreset(key);
    }
    virtual IMessage* createMessageSetChannelTracking(Global::ChannelTracking method) {
        return new MessageSetChannelTracking(method);
    }
    virtual IMessage* createMessageSetEnableOverCurrentProtection(bool enable) {
        return new MessageSetEnableOverCurrentProtection(enable);
    }
    virtual IMessage* createMessageSetEnableOverVoltageProtection(bool enable) {
        return new MessageSetEnableOverVoltageProtection(enable);
    }
    virtual IMessage* createMessageSetOverCurrentProtectionValue(Global::Channel channel, double current) {
        return new MessageSetOverCurrentProtectionValue(channel, current);
    }
    virtual IMessage* createMessageGetOverCurrentProtectionValue(Global::Channel channel) {
        return new MessageGetOverCurrentProtectionValue(channel);
    }
    virtual IMessage* createMessageSetOverVoltageProtectionValue(Global::Channel channel, double voltage) {
        return new MessageSetOverVoltageProtectionValue(channel, voltage);
    }
    virtual IMessage* createMessageGetOverVoltageProtectionValue(Global::Channel channel) {
        return new MessageGetOverVoltageProtectionValue(channel);
    }

protected:
    virtual Global::OutputMode evaluateOutputMode(QByteArray data, Global::Channel channel) const {
        return channel == Global::Channel1
            ? Global::OutputMode(bool(data[0] & 0x1))
            : Global::OutputMode(bool(data[0] & 0x2));
    }

    virtual Global::ChannelTracking evaluateChannelTracking(const QByteArray &data) const {
        Global::ChannelTracking tracking = Global::Independent;
        tracking = bool(data[0] & 0x4) ? Global::Serial : tracking;
        tracking = bool(data[0] & 0x8) ? Global::Parallel : tracking;

        return tracking;
    }

    virtual Global::OutputProtection evaluateOutputProtection(const QByteArray &data) const {
        Global::OutputProtection protection = Global::OutputProtectionAllDisabled;
        protection = bool(data[0] & 0x10) ? Global::OverVoltageProtectionOnly : protection;
        protection = bool(data[0] & 0x20) ? Global::OverCurrentProtectionOnly : protection;
        protection = bool((data[0] & 0x30) == 0x30) ? Global::OutputProtectionAllEnabled : protection;

        return protection;
    }

    virtual bool evaluateOutputSwitchState(const QByteArray &data) const {
        return bool(data[0] & 0x40);
    }
};
}

#endif //PSM_BASESCPI_H
