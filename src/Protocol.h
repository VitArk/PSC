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
     * Lock power supply operation panel
     * Example:LOCK0
     * Unlock power supply operation panel
     */
    class Lock : public IProtocol {
    public:
        Lock(bool lock) {
            mLock = lock;
        }

        QByteArray requestQuery() const override {
            return QByteArray("LOCK").append(QByteArray::number((int)mLock));
        }
    private:
        bool mLock;
    };

    /**
     * ISET<X>: <NR2>
     * Function Description: Set current value
     * Example: ISET1:2.225
     * Set current value as 2.225A
     */
    class SetCurrent : public IProtocol {
    public:
        SetCurrent(TChannel channel, double value) {
            mChannel = channel;
            mValue = value;
        }

        QByteArray requestQuery() const override {
            return QString("ISET%1:%2").arg(mChannel).arg(mValue).toLatin1();
        }

    private:
        double   mValue;
    };

    /**
     * ISET<X>?
     * Function Description: Read the set current value
     * Example: ISET1?
     * Returns current value
     */
    class ReadSetCurrent : public IProtocol {
    public:
        ReadSetCurrent(TChannel channel) {
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
        SetVoltage(TChannel channel, double value) {
            mChannel = channel;
            mValue = value;
        }

        QByteArray requestQuery() const override {
            return QString("VSET%1:%2").arg(mChannel).arg(mValue).toLatin1();
        }

    private:
        double   mValue;
    };

    /**
     * VSET<X>?
     * Function Description: Read the set voltage value
     * Example: VSET1?
     * Returns voltage value
     */
    class ReadSetVoltage : public IProtocol {
    public:
        ReadSetVoltage(TChannel channel) {
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
     * Function Description: Read current output value
     * Example: IOUT1?
     * Read the set current value
     */
    class ReadOutputCurrent : public IProtocol {
    public:
        ReadOutputCurrent(TChannel channel) {
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
    * Function Description: Read voltage output value
    * Example: VOUT1?
    * Read the set voltage value
    */
    class ReadOutputVoltage : public IProtocol {
    public:
        ReadOutputVoltage(TChannel channel) {
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
    * Function Description: Turn on/off power supply output
    * Boolean: 0 off; 1 on
    * Example: OUT1 Turn on power supply output
    */
    class Output : public IProtocol {
    public:
        Output(bool state) {
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
    * Function Description: Turn on/off buzzer
    * Example: BEEP1 Turn on buzzer
    */
    class Buzzer : public IProtocol {
    public:
        Buzzer(bool state) {
            mState = state;
        }

        QByteArray requestQuery() const override {
            return QString("BEEP%1").arg((int)mState).toLatin1();
        }
    private:
        bool mState;
    };

    /**
     * STATUS?
     * Function Description: Read power supply output status
     * Contents 8 bits in the following format:
     *  Bit     Item    Description
     *  0       CH1     0=CC mode, 1=CV mode
     *  1       CH2     0=CC mode, 1=CV mode
     *  2,3,4,5 N/A     N/A
     *  6       Output  0=Off, 1=On
     *  7       N/A     N/A
     */
    class ReadOutputStatus : public IProtocol {
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
    class ReadDeviceInfo : public IProtocol {
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
    class ReadActiveSetting : public IProtocol {
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
     * Function Description: Set series & parallel output
     * NR1: 0=independent output; 1=series output; 2=parallel output
     * Example: TRACK1
     */
    class OutputMode : public IProtocol {
    public:
        OutputMode(TOutputMode mode) {
            mMode = mode;
        }

        QByteArray requestQuery() const override {
            return QString("TRACK%1").arg(mMode).toLatin1();
        }
    private:
        TOutputMode mMode;
    };

    /**
     * OCP<Boolean>
     * Function Description: Turn on over current protection
     * Boolean: 0 OFF, 1 ON
     * Example: OCP1 Turn on OCP
     */
    class OverCurrentProtection : public IProtocol {
    public:
        OverCurrentProtection(bool state) {
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
    class OverVoltageProtection : public IProtocol {
    public:
        OverVoltageProtection(bool state) {
            mState = state;
        }

        QByteArray requestQuery() const override {
            return QString("OVP%1").arg((int)mState).toLatin1();
        }
    private:
        bool mState;
    };

    /**
    * OCPSTE:<X>:<NR2>
    * Function Description: Set OCP value
    * Example: OCPSTE1: 5.100
    */
    class SetOverCurrentProtection : public IProtocol {
    public:
        SetOverCurrentProtection(TChannel channel, double value) {
            mChannel = channel;
            mValue = value;
        }

        QByteArray requestQuery() const override {
            return QString("OCPSTE%1:%2").arg(mChannel).arg(mValue).toLatin1();
        }

    private:
        double   mValue;
    };

    /**
     * OVPSTE:<X>:<NR2>
     * Function Description: Set OVP value
     * Example: OVPSTE1:31.00
     */
    class SetOverVoltageProtection : public IProtocol {
    public:
        SetOverVoltageProtection(TChannel channel, double value) {
            mChannel = channel;
            mValue = value;
        }

        QByteArray requestQuery() const override {
            return QString("OVPSTE%1:%2").arg(mChannel).arg(mValue).toLatin1();
        }

    private:
        double   mValue;
    };
}





#endif //PSC_PROTOCOL_H
