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

#ifndef PSM_DEVICE_H
#define PSM_DEVICE_H

#include <QString>
#include "Commons.h"
#include "Messages.h"

namespace Protocol {
    class Interface {
    public:
        virtual ~Interface() = default;
        virtual QString name() const = 0;
        virtual QString id() const = 0;
        virtual QString description() const = 0;
        virtual double maxChannelCurrent() const = 0;
        virtual double maxChannelVoltage() const = 0;
        virtual int activeChannels() const = 0;

        virtual bool checkID(QString id) const {
            return id == this->id();
        }

        virtual DeviceInfo info() const {
            return {
                .name = name(),
                .ID = id(),
                .description = description(),
                .maxChannelCurrent = maxChannelCurrent(),
                .maxChannelVoltage = maxChannelVoltage(),
                .activeChannels = activeChannels(),
            };
        }

        virtual IMessage* createMessageSetLocked(bool lock) {
            return new MessageSetLocked(lock);
        }
        virtual IMessage* createMessageGetIsLocked() {
            return new MessageGetIsLocked();
        }
        virtual IMessage* createMessageSetCurrent(TChannel channel, double value) {
            return new MessageSetCurrent(channel, value);
        }
        virtual IMessage* createMessageGetCurrentSet(TChannel channel) {
            return new MessageGetCurrentSet(channel);
        }
        virtual IMessage* createMessageSetVoltage(TChannel channel, double value) {
            return new MessageSetVoltage(channel, value);
        }
        virtual IMessage* createMessageGetVoltageSet(TChannel channel) {
            return new MessageGetVoltageSet(channel);
        }
        virtual IMessage* createMessageGetActualCurrent(TChannel channel) {
            return new MessageGetActualCurrent(channel);
        }
        virtual IMessage* createMessageGetActualVoltage(TChannel channel) {
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
        virtual IMessage* createMessageSetPreset(TMemoryKey key) {
            return new MessageSetPreset(key);
        }
        virtual IMessage* createMessageGetPreset() {
            return new MessageGetPreset();
        }
        virtual IMessage* createMessageSavePreset(TMemoryKey key) {
            return new MessageSavePreset(key);
        }
        virtual IMessage* createMessageSetChannelTracking(TChannelTracking method) {
            return new MessageSetChannelTracking(method);
        }
        virtual IMessage* createMessageSetEnableOverCurrentProtection(bool enable) {
            return new MessageSetEnableOverCurrentProtection(enable);
        }
        virtual IMessage* createMessageSetEnableOverVoltageProtection(bool enable) {
            return new MessageSetEnableOverVoltageProtection(enable);
        }
        virtual IMessage* createMessageSetOverCurrentProtectionValue(TChannel channel, double current) {
            return new MessageSetOverCurrentProtectionValue(channel, current);
        }
        virtual IMessage* createMessageGetOverCurrentProtectionValue(TChannel channel) {
            return new MessageGetOverCurrentProtectionValue(channel);
        }
        virtual IMessage* createMessageSetOverVoltageProtectionValue(TChannel channel, double voltage) {
            return new MessageSetOverVoltageProtectionValue(channel, voltage);
        }
        virtual IMessage* createMessageGetOverVoltageProtectionValue(TChannel channel) {
            return new MessageGetOverVoltageProtectionValue(channel);
        }
    };
}

#endif //PSM_DEVICE_H
