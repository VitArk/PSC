//
// Created by Vitalii Arkusha on 14.10.2022.
//

#ifndef PSM_DEVICE_H
#define PSM_DEVICE_H

#include <QString>
#include "Commons.h"
#include "Protocol.h"

namespace Protocol {
    class Device {
    public:
        virtual ~Device() = default;
        virtual QString name() const = 0;
        virtual QString id() const = 0;
        virtual QString description() const = 0;
        virtual double maxChannelCurrent() const = 0;
        virtual double maxChannelVoltage() const = 0;

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
            };
        }

        virtual IMessage* createRequestLockOperationPanel(bool lock) {
            return new LockOperationPanel(lock);
        }
        virtual IMessage* createRequestIsOperationPanelLocked() {
            return new IsOperationPanelLocked();
        }
        virtual IMessage* createRequestSetCurrent(TChannel channel, double value) {
            return new SetCurrent(channel, value);
        }
        virtual IMessage* createRequestGetCurrent(TChannel channel) {
            return new GetCurrent(channel);
        }
        virtual IMessage* createRequestSetVoltage(TChannel channel, double value) {
            return new SetVoltage(channel, value);
        }
        virtual IMessage* createRequestGetVoltage(TChannel channel) {
            return new GetVoltage(channel);
        }
        virtual IMessage* createRequestGetOutputCurrent(TChannel channel) {
            return new GetOutputCurrent(channel);
        }
        virtual IMessage* createRequestGetOutputVoltage(TChannel channel) {
            return new GetOutputVoltage(channel);
        }
        virtual IMessage* createRequestEnableOutputSwitch(bool enable) {
            return new EnableOutputSwitch(enable);
        }
        virtual IMessage* createRequestEnableBuzzer(bool enable) {
            return new EnableBuzzer(enable);
        }
        virtual IMessage* createRequestIsBuzzerEnabled() {
            return new IsBuzzerEnabled();
        }
        virtual IMessage* createRequestGetDeviceStatus() {
            return new GetDeviceStatus();
        }
        virtual IMessage* createRequestGetDeviceID() {
            return new GetDeviceID();
        }
        virtual IMessage* createRequestApplySettings(TMemoryKey key) {
            return new ApplySettings(key);
        }
        virtual IMessage* createRequestGetActiveSettings() {
            return new GetActiveSettings();
        }
        virtual IMessage* createRequestSaveSettings(TMemoryKey key) {
            return new SaveSettings(key);
        }
        virtual IMessage* createRequestChangeOutputConnectionMethod(TOutputConnectionMethod method) {
            return new ChangeOutputConnectionMethod(method);
        }
        virtual IMessage* createRequestEnableOverCurrentProtection(bool enable) {
            return new EnableOverCurrentProtection(enable);
        }
        virtual IMessage* createRequestEnableOverVoltageProtection(bool enable) {
            return new EnableOverVoltageProtection(enable);
        }
        virtual IMessage* createRequestSetOverCurrentProtectionValue(TChannel channel, double current) {
            return new SetOverCurrentProtectionValue(channel, current);
        }
        virtual IMessage* createRequestGetOverCurrentProtectionValue(TChannel channel) {
            return new GetOverCurrentProtectionValue(channel);
        }
        virtual IMessage* createRequestSetOverVoltageProtectionValue(TChannel channel, double voltage) {
            return new SetOverVoltageProtectionValue(channel, voltage);
        }
        virtual IMessage* createRequestGetOverVoltageProtectionValue(TChannel channel) {
            return new GetOverVoltageProtectionValue(channel);
        }
    };
}

#endif //PSM_DEVICE_H
