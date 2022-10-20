//
// Created by Vitalii Arkusha on 14.10.2022.
//

#ifndef PSM_DEVICE_H
#define PSM_DEVICE_H

#include <QString>
#include "Commons.h"
#include "Messages.h"

namespace Protocol {
    class Device {
    public:
        virtual ~Device() = default;
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

        virtual IMessage* createRequestLockOperationPanel(bool lock) {
            return new MessageLockOperationPanel(lock);
        }
        virtual IMessage* createRequestIsOperationPanelLocked() {
            return new MessageIsOperationPanelLocked();
        }
        virtual IMessage* createRequestSetCurrent(TChannel channel, double value) {
            return new MessageSetCurrent(channel, value);
        }
        virtual IMessage* createRequestGetCurrent(TChannel channel) {
            return new MessageGetCurrent(channel);
        }
        virtual IMessage* createRequestSetVoltage(TChannel channel, double value) {
            return new MessageSetVoltage(channel, value);
        }
        virtual IMessage* createRequestGetVoltage(TChannel channel) {
            return new MessageGetVoltage(channel);
        }
        virtual IMessage* createRequestGetOutputCurrent(TChannel channel) {
            return new MessageGetOutputCurrent(channel);
        }
        virtual IMessage* createRequestGetOutputVoltage(TChannel channel) {
            return new MessageGetOutputVoltage(channel);
        }
        virtual IMessage* createRequestEnableOutputSwitch(bool enable) {
            return new MessageEnableOutputSwitch(enable);
        }
        virtual IMessage* createRequestEnableBuzzer(bool enable) {
            return new MessageEnableBuzzer(enable);
        }
        virtual IMessage* createRequestIsBuzzerEnabled() {
            return new MessageIsBuzzerEnabled();
        }
        virtual IMessage* createRequestGetDeviceStatus() {
            return new MessageGetDeviceStatus();
        }
        virtual IMessage* createRequestGetDeviceID() {
            return new MessageGetDeviceID();
        }
        virtual IMessage* createRequestSetPreset(TMemoryKey key) {
            return new MessageSetPreset(key);
        }
        virtual IMessage* createRequestGetPreset() {
            return new MessageGetPreset();
        }
        virtual IMessage* createRequestSavePreset(TMemoryKey key) {
            return new MessageSavePreset(key);
        }
        virtual IMessage* createRequestChangeOutputConnectionMethod(TOutputConnectionMethod method) {
            return new MessageChangeOutputConnectionMethod(method);
        }
        virtual IMessage* createRequestEnableOverCurrentProtection(bool enable) {
            return new MessageEnableOverCurrentProtection(enable);
        }
        virtual IMessage* createRequestEnableOverVoltageProtection(bool enable) {
            return new MessageEnableOverVoltageProtection(enable);
        }
        virtual IMessage* createRequestSetOverCurrentProtectionValue(TChannel channel, double current) {
            return new MessageSetOverCurrentProtectionValue(channel, current);
        }
        virtual IMessage* createRequestGetOverCurrentProtectionValue(TChannel channel) {
            return new MessageGetOverCurrentProtectionValue(channel);
        }
        virtual IMessage* createRequestSetOverVoltageProtectionValue(TChannel channel, double voltage) {
            return new MessageSetOverVoltageProtectionValue(channel, voltage);
        }
        virtual IMessage* createRequestGetOverVoltageProtectionValue(TChannel channel) {
            return new MessageGetOverVoltageProtectionValue(channel);
        }
    };
}

#endif //PSM_DEVICE_H
