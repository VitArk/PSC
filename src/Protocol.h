//
// Created by Vitalii Arkusha on 17.09.2022.
//

#ifndef PSC_PROTOCOL_H
#define PSC_PROTOCOL_H

#include <QString>
#include "Commons.h"

namespace Protocol {
    class IProtocol {
    public:
        TChannel channel() const { return mChannel;}
        virtual QByteArray requestQuery() const = 0;
        virtual int responseDataSize() const { return 0; }
        virtual ~IProtocol() = default;
    protected:
        TChannel mChannel;
    };

    /**
     * LOCK<NR2>
     * Function Description:Lock power supply operation panel
     * Example: LOCK1
     * LockOperationPanel power supply operation panel
     * Example:LOCK0
     * Unlock power supply operation panel
     */
    class LockOperationPanel : public IProtocol {
    public:
        LockOperationPanel(bool lock) {
            mLock = lock;
        }

        QByteArray requestQuery() const override {
            return QByteArray("LOCK").append(QByteArray::number((int)mLock));
        }
    private:
        bool mLock;
    };

    /**
     * LOCK?
     * Function Description: Check lock status of power supply operation panel
     * Example: LOCK?
     * Response: 0 | 1
     */
    class IsOperationPanelLocked : public IProtocol {
    public:
        QByteArray requestQuery() const override {
            return "LOCK?";
        }

        int responseDataSize() const override {
            return 1;
        };
    };


    /**
     * ISET<X>: <NR2>
     * Function Description: Set current value
     * Example: ISET1:2.225
     * Set current value as 2.225A
     */
    class SetCurrent : public IProtocol {
    public:
        SetCurrent(TChannel channel, double current) {
            mChannel = channel;
            mCurrent = current;
        }

        QByteArray requestQuery() const override {
            return QString("ISET%1:%2").arg(mChannel).arg(mCurrent).toLatin1();
        }

    private:
        double   mCurrent;
    };

    /**
     * ISET<X>?
     * Function Description: Read the set current value
     * Example: ISET1?
     * Returns current value
     */
    class GetCurrent : public IProtocol {
    public:
        GetCurrent(TChannel channel) {
            mChannel = channel;
        }

        QByteArray requestQuery() const override {
            return QString("ISET%1?").arg(mChannel).toLatin1();
        }

        int responseDataSize() const override {
            return 5;
        };
    };

    /**
     * VSET<X>:<NR2>
     * Function Description: Set voltage value
     * Example: VSET1:20.50
     * Set voltage value for channel 1 as 20.50V
     */
    class SetVoltage : public IProtocol {
    public:
        SetVoltage(TChannel channel, double voltage) {
            mChannel = channel;
            mVoltage = voltage;
        }

        QByteArray requestQuery() const override {
            return QString("VSET%1:%2").arg(mChannel).arg(mVoltage).toLatin1();
        }

    private:
        double   mVoltage;
    };

    /**
     * VSET<X>?
     * Function Description: Read the set voltage value
     * Example: VSET1?
     * Returns voltage value
     */
    class GetVoltage : public IProtocol {
    public:
        GetVoltage(TChannel channel) {
            mChannel = channel;
        }

        QByteArray requestQuery() const override {
            return QString("VSET%1?").arg(mChannel).toLatin1();
        }

        int responseDataSize() const override {
            return 5;
        };
    };

    /**
     * IOUT<X>?
     * Function Description: Read current setOutputSwitch value
     * Example: IOUT1?
     * Read the set current value
     */
    class GetOutputCurrent : public IProtocol {
    public:
        GetOutputCurrent(TChannel channel) {
            mChannel = channel;
        }

        QByteArray requestQuery() const override {
            return QString("IOUT%1?").arg(mChannel).toLatin1();
        }

        int responseDataSize() const override {
            return 5;
        };
    };

    /**
    * VOUT<X>?
    * Function Description: Read voltage setOutputSwitch value
    * Example: VOUT1?
    * Read the set voltage value
    */
    class GetOutputVoltage : public IProtocol {
    public:
        GetOutputVoltage(TChannel channel) {
            mChannel = channel;
        }

        QByteArray requestQuery() const override {
            return QString("VOUT%1?").arg(mChannel).toLatin1();
        }

        int responseDataSize() const override {
            return 5;
        };
    };

    /**
    * OUT<Boolean>
    * Function Description: Turn on/off power supply setOutputSwitch
    * Boolean: 0 off; 1 on
    * Example: OUT1 Turn on power supply setOutputSwitch
    */
    class SetOutputSwitch : public IProtocol {
    public:
        SetOutputSwitch(bool state) {
            mState = state;
        }

        QByteArray requestQuery() const override {
            return QString("OUT%1").arg((int)mState).toLatin1();
        }
    private:
        bool mState;
    };

    /**
    * BEEP<Boolean>
    * Function Description: Turn on/off enableBeep
    * Example: BEEP1 Turn on enableBeep
    */
    class EnableBuzzer : public IProtocol {
    public:
        EnableBuzzer(bool state) {
            mState = state;
        }

        QByteArray requestQuery() const override {
            return QString("BEEP%1").arg((int)mState).toLatin1();
        }
    private:
        bool mState;
    };

    /**
     * BEEP?
     * Function Description: Check buzzer on/off status
     * Example: BEEP?
     * Response: 1 | 0
     */
    class IsBuzzerEnabled : public IProtocol {
    public:
        QByteArray requestQuery() const override {
            return "BEEP?";
        }
        int responseDataSize() const override {
            return 1;
        };
    };

    /**
     * STATUS?
     * Function Description: Read power supply setOutputSwitch status
     * Contents 8 bits in the following format:
     *  Bit     Item            Description
     *  0       CH1             0=CC mode, 1=CV mode
     *  1       CH2             0=CC mode, 1=CV mode
     *  2       SerialMode      0=Off, 1=On
     *  3       ParallelMode    0=Off, 1=On
     *  4       OVP             0=Off, 1=On
     *  5       OCP             0=Off, 1=On
     *  6       setOutputSwitch 0=Off, 1=On
     *  7       N/A             N/A
     *
     *  ** if bits (2=0 and 3=0) -- Independent method.
     */
    class GetOutputStatus : public IProtocol {
    public:
        QByteArray requestQuery() const override {
            return "STATUS?";
        }
        int responseDataSize() const override {
            return 1;
        };
    };

    /**
     * *IDN?
     * Function Description: Return to device model & factory information
     * Example: *IDN?
     * Contents UNI-T P33XC V2.0 (manufacturer, model name)
     */
    class GetDeviceInfo : public IProtocol {
    public:
        QByteArray requestQuery() const override {
            return "*IDN?";
        }

        int responseDataSize() const override {
            return 9;
        };
    };

    /**
     * RCL<NR1>
     * Function Description:Storage recall by pressing keys from M1-M5
     */
    class RecallSetting : public IProtocol {
    public:
        RecallSetting(TMemoryKey key) {
            mKey = key;
        }

        QByteArray requestQuery() const override {
            return QString("RCL%1").arg(mKey).toLatin1();
        }
    private:
        TMemoryKey mKey;
    };

    /**
     * RCL?
     * Function Description:Read current/active setting number (keys from M1-M5)
     */
    class GetRecalledSetting : public IProtocol {
    public:
        QByteArray requestQuery() const override {
            return "RCL?";
        }
        int responseDataSize() const override {
            return 1;
        };
    };

    /**
     * SAV<NR1>
     * Function Description: Storage setting
     * Example: SAV1 Stores the panel setting in memory number 1
     */
    class SaveSetting : public IProtocol {
    public:
        SaveSetting(TMemoryKey key) {
            mKey = key;
        }

        QByteArray requestQuery() const override {
            return QString("SAV%1").arg(mKey).toLatin1();
        }
    private:
        TMemoryKey mKey;
    };

    /**
     * TRACK<NR1>
     * Function Description: Set series & parallel setOutputSwitch
     * NR1: 0=independent output; 1=series output; 2=parallel setOutputSwitch
     * Example: TRACK1
     */
    class OutputConnectionMethod : public IProtocol {
    public:
        OutputConnectionMethod(TOutputConnectionMethod mode) {
            mMode = mode;
        }

        QByteArray requestQuery() const override {
            return QString("TRACK%1").arg(mMode).toLatin1();
        }
    private:
        TOutputConnectionMethod mMode;
    };

    /**
     * OCP<Boolean>
     * Function Description: Turn on over current protection
     * Boolean: 0 OFF, 1 ON
     * Example: OCP1 Turn on OCP
     */
    class EnableOverCurrentProtection : public IProtocol {
    public:
        EnableOverCurrentProtection(bool state) {
            mState = state;
        }

        QByteArray requestQuery() const override {
            return QString("OCP%1").arg((int)mState).toLatin1();
        }
    private:
        bool mState;
    };


    /**
     * OVP<Boolean>
     * Function Description: Turn on over voltage protection
     * Boolean: 0 OFF, 1 ON
     * Example: OVP1 Turn on OVP
     */
    class EnableOverVoltageProtection : public IProtocol {
    public:
        EnableOverVoltageProtection(bool state) {
            mState = state;
        }

        QByteArray requestQuery() const override {
            return QString("OVP%1").arg((int)mState).toLatin1();
        }
    private:
        bool mState;
    };

    /**
    * OCPSET:<X>:<NR2>
    * Function Description: Set OCP value
    * Example: OCPSET1: 5.100
    */
    class SetOverCurrentProtectionValue : public IProtocol {
    public:
        SetOverCurrentProtectionValue(TChannel channel, double value) {
            mChannel = channel;
            mValue = value;
        }

        QByteArray requestQuery() const override {
            return QString("OCPSET%1:%2").arg(mChannel).arg(mValue).toLatin1();
        }

    private:
        double   mValue;
    };

    /**
    * OCPSET<X>?
    * Function Description: Get OCP value
    * Example: OCPSET1?
    */
    class GetOverCurrentProtectionValue : public IProtocol {
    public:
        GetOverCurrentProtectionValue(TChannel channel) {
            mChannel = channel;
        }

        QByteArray requestQuery() const override {
            return QString("OCPSET%1?").arg(mChannel).toLatin1();
        }

        int responseDataSize() const override {
            return 5;
        };
    };

    /**
     * OVPSET:<X>:<NR2>
     * Function Description: Set OVP value
     * Example: OVPSET1:31.00
     */
    class SetOverVoltageProtectionValue : public IProtocol {
    public:
        SetOverVoltageProtectionValue(TChannel channel, double voltage) {
            mChannel = channel;
            mVoltage = voltage;
        }

        QByteArray requestQuery() const override {
            return QString("OVPSET%1:%2").arg(mChannel).arg(mVoltage).toLatin1();
        }

    private:
        double   mVoltage;
    };

    /**
     * OVPSET:<X>?
     * Function Description: Grt OVP value
     * Example: OVPSET1?
     */
    class GetOverVoltageProtectionValue : public IProtocol {
    public:
        GetOverVoltageProtectionValue(TChannel channel) {
            mChannel = channel;
        }

        QByteArray requestQuery() const override {
            return QString("OVPSET%1?").arg(mChannel).toLatin1();
        }

        int responseDataSize() const override {
            return 5;
        };
    };
}





#endif //PSC_PROTOCOL_H
