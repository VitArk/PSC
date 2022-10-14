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

        virtual IRequest* createRequestLockOperationPanel(bool lock) {
            return new LockOperationPanel(lock);
        }
        virtual IRequest* createRequestIsOperationPanelLocked() {
            return new IsOperationPanelLocked();
        }
        virtual IRequest* createRequestSetCurrent(TChannel channel, double value) {
            return new SetCurrent(channel, value);
        }
        virtual IRequest* createRequestGetCurrent(TChannel channel) {
            return new GetCurrent(channel);
        }
        virtual IRequest* createRequestSetVoltage(TChannel channel, double value) {
            return new SetVoltage(channel, value);
        }
        virtual IRequest* createRequestGetVoltage(TChannel channel) {
            return new GetVoltage(channel);
        }
        virtual IRequest* createRequestGetOutputCurrent(TChannel channel) {
            return new GetOutputCurrent(channel);
        }
        virtual IRequest* createRequestGetOutputVoltage(TChannel channel) {
            return new GetOutputVoltage(channel);
        }
        virtual IRequest* createRequestEnableOutputSwitch(bool enable) {
            return new EnableOutputSwitch(enable);
        }
        virtual IRequest* createRequestEnableBuzzer(bool enable) {
            return new EnableBuzzer(enable);
        }
        virtual IRequest* createRequestIsBuzzerEnabled() {
            return new IsBuzzerEnabled();
        }
        virtual IRequest* createRequestGetDeviceStatus() {
            return new GetDeviceStatus();
        }
        virtual IRequest* createRequestGetDeviceID() {
            return new GetDeviceID();
        }
        virtual IRequest* createRequestApplySettings(TMemoryKey key) {
            return new ApplySettings(key);
        }
        virtual IRequest* createRequestGetActiveSettings() {
            return new GetActiveSettings();
        }
        virtual IRequest* createRequestSaveSettings(TMemoryKey key) {
            return new SaveSettings(key);
        }
        virtual IRequest* createRequestChangeOutputConnectionMethod(TOutputConnectionMethod method) {
            return new ChangeOutputConnectionMethod(method);
        }
        virtual IRequest* createRequestEnableOverCurrentProtection(bool enable) {
            return new EnableOverCurrentProtection(enable);
        }
        virtual IRequest* createRequestEnableOverVoltageProtection(bool enable) {
            return new EnableOverVoltageProtection(enable);
        }
        virtual IRequest* createRequestSetOverCurrentProtectionValue(TChannel channel, double current) {
            return new SetOverCurrentProtectionValue(channel, current);
        }
        virtual IRequest* createRequestGetOverCurrentProtectionValue(TChannel channel) {
            return new GetOverCurrentProtectionValue(channel);
        }
        virtual IRequest* createRequestSetOverVoltageProtectionValue(TChannel channel, double voltage) {
            return new SetOverVoltageProtectionValue(channel, voltage);
        }
        virtual IRequest* createRequestGetOverVoltageProtectionValue(TChannel channel) {
            return new GetOverVoltageProtectionValue(channel);
        }
    };
}

#endif //PSM_DEVICE_H
