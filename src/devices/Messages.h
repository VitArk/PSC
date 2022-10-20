//
// Created by Vitalii Arkusha on 17.09.2022.
//

#ifndef PSC_PROTOCOL_H
#define PSC_PROTOCOL_H

#include <QString>
#include "Commons.h"

namespace Protocol {
    class IMessage {
    public:
        virtual TChannel channel() const { return mChannel;}
        virtual QByteArray query() const = 0;
        virtual int answerLength() const { return 0; }
        virtual ~IMessage() = default;
        virtual bool isQuery() const { return answerLength() > 0; }
        virtual bool isCommand() const { return answerLength() == 0; }
        // messages that return true, will be dropped in case overflowing messages queue.
        virtual bool allowDrop() const { return false; }
    protected:
        TChannel mChannel;
    };

    /**
     * LOCK<NR2>
     * Function Description:Lock power supply operation panel
     * Example: LOCK1
     * MessageLockOperationPanel power supply operation panel
     * Example:LOCK0
     * Unlock power supply operation panel
     */
    class MessageLockOperationPanel : public IMessage {
    public:
        MessageLockOperationPanel(bool lock) {
            mLock = lock;
        }

        QByteArray query() const override {
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
    class MessageIsOperationPanelLocked : public IMessage {
    public:
        QByteArray query() const override {
            return "LOCK?";
        }

        int answerLength() const override {
            return 1;
        };

        bool allowDrop() const override {
            return true;
        }
    };


    /**
     * ISET<X>: <NR2>
     * Function Description: Set current value
     * Example: ISET1:2.225
     * Set current value as 2.225A
     */
    class MessageSetCurrent : public IMessage {
    public:
        MessageSetCurrent(TChannel channel, double current) {
            mChannel = channel;
            mCurrent = current;
        }

        QByteArray query() const override {
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
    class MessageGetCurrent : public IMessage {
    public:
        MessageGetCurrent(TChannel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("ISET%1?").arg(mChannel).toLatin1();
        }

        int answerLength() const override {
            return 5;
        };
    };

    /**
     * VSET<X>:<NR2>
     * Function Description: Set voltage value
     * Example: VSET1:20.50
     * Set voltage value for channel 1 as 20.50V
     */
    class MessageSetVoltage : public IMessage {
    public:
        MessageSetVoltage(TChannel channel, double voltage) {
            mChannel = channel;
            mVoltage = voltage;
        }

        QByteArray query() const override {
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
    class MessageGetVoltage : public IMessage {
    public:
        MessageGetVoltage(TChannel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("VSET%1?").arg(mChannel).toLatin1();
        }

        int answerLength() const override {
            return 5;
        };
    };

    /**
     * IOUT<X>?
     * Function Description: Read current setEnableOutputSwitch value
     * Example: IOUT1?
     * Read the set current value
     */
    class MessageGetOutputCurrent : public IMessage {
    public:
        MessageGetOutputCurrent(TChannel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("IOUT%1?").arg(mChannel).toLatin1();
        }

        int answerLength() const override {
            return 5;
        };
    };

    /**
    * VOUT<X>?
    * Function Description: Read voltage setEnableOutputSwitch value
    * Example: VOUT1?
    * Read the set voltage value
    */
    class MessageGetOutputVoltage : public IMessage {
    public:
        MessageGetOutputVoltage(TChannel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("VOUT%1?").arg(mChannel).toLatin1();
        }

        int answerLength() const override {
            return 5;
        };
    };

    /**
    * OUT<Boolean>
    * Function Description: Turn on/off power supply setEnableOutputSwitch
    * Boolean: 0 off; 1 on
    * Example: OUT1 Turn on power supply setEnableOutputSwitch
    */
    class MessageEnableOutputSwitch : public IMessage {
    public:
        MessageEnableOutputSwitch(bool state) {
            mState = state;
        }

        QByteArray query() const override {
            return QString("OUT%1").arg((int)mState).toLatin1();
        }
    private:
        bool mState;
    };

    /**
    * BEEP<Boolean>
    * Function Description: Turn on/off enableBuzzer
    * Example: BEEP1 Turn on enableBuzzer
    */
    class MessageEnableBuzzer : public IMessage {
    public:
        MessageEnableBuzzer(bool state) {
            mState = state;
        }

        QByteArray query() const override {
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
    class MessageIsBuzzerEnabled : public IMessage {
    public:
        QByteArray query() const override {
            return "BEEP?";
        }
        int answerLength() const override {
            return 1;
        };
        bool allowDrop() const override {
            return true;
        }
    };

    /**
     * STATUS?
     * Function Description: Read power supply setEnableOutputSwitch status
     * Contents 8 bits in the following format:
     *  Bit     Item            Description
     *  0       CH1             0=CC mode, 1=CV mode
     *  1       CH2             0=CC mode, 1=CV mode
     *  2       SerialMode      0=Off, 1=On
     *  3       ParallelMode    0=Off, 1=On
     *  4       OVP             0=Off, 1=On
     *  5       OCP             0=Off, 1=On
     *  6       OutputSwitch    0=Off, 1=On
     *  7       N/A             N/A
     *
     *  ** if bits (2=0 and 3=0) -- Independent method.
     */
    class MessageGetDeviceStatus : public IMessage {
    public:
        QByteArray query() const override {
            return "STATUS?";
        }
        int answerLength() const override {
            return 1;
        };
    };

    /**
     * *IDN?
     * Function Description: Return to device model & factory information
     * Example: *IDN?
     * Contents UNI-T P33XC V2.0 (manufacturer, model name)
     */
    class MessageGetDeviceID : public IMessage {
    public:
        QByteArray query() const override {
            return "*IDN?";
        }

        int answerLength() const override {
            return 9;
        };
    };

    /**
     * RCL<NR1>
     * Function Description:Storage recall by pressing keys from M1-M5
     */
    class MessageSetPreset : public IMessage {
    public:
        MessageSetPreset(TMemoryKey key) {
            mKey = key;
        }

        QByteArray query() const override {
            return QString("RCL%1").arg(mKey).toLatin1();
        }
    private:
        TMemoryKey mKey;
    };

    /**
     * RCL?
     * Function Description:Read current/active setting number (keys from M1-M5)
     */
    class MessageGetPreset : public IMessage {
    public:
        QByteArray query() const override {
            return "RCL?";
        }
        int answerLength() const override {
            return 1;
        };
        bool allowDrop() const override {
            return true;
        }
    };

    /**
     * SAV<NR1>
     * Function Description: Storage setting
     * Example: SAV1 Stores the panel setting in memory number 1
     */
    class MessageSavePreset : public IMessage {
    public:
        MessageSavePreset(TMemoryKey key) {
            mKey = key;
        }

        QByteArray query() const override {
            return QString("SAV%1").arg(mKey).toLatin1();
        }
    private:
        TMemoryKey mKey;
    };

    /**
     * TRACK<NR1>
     * Function Description: Set series & parallel setEnableOutputSwitch
     * NR1: 0=independent output; 1=series output; 2=parallel setEnableOutputSwitch
     * Example: TRACK1
     */
    class MessageChangeOutputConnectionMethod : public IMessage {
    public:
        MessageChangeOutputConnectionMethod(TOutputConnectionMethod mode) {
            mMode = mode;
        }

        QByteArray query() const override {
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
    class MessageEnableOverCurrentProtection : public IMessage {
    public:
        MessageEnableOverCurrentProtection(bool state) {
            mState = state;
        }

        QByteArray query() const override {
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
    class MessageEnableOverVoltageProtection : public IMessage {
    public:
        MessageEnableOverVoltageProtection(bool state) {
            mState = state;
        }

        QByteArray query() const override {
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
    class MessageSetOverCurrentProtectionValue : public IMessage {
    public:
        MessageSetOverCurrentProtectionValue(TChannel channel, double value) {
            mChannel = channel;
            mValue = value;
        }

        QByteArray query() const override {
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
    class MessageGetOverCurrentProtectionValue : public IMessage {
    public:
        MessageGetOverCurrentProtectionValue(TChannel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("OCPSET%1?").arg(mChannel).toLatin1();
        }

        int answerLength() const override {
            return 5;
        };

        bool allowDrop() const override {
            return true;
        }
    };

    /**
     * OVPSET:<X>:<NR2>
     * Function Description: Set OVP value
     * Example: OVPSET1:31.00
     */
    class MessageSetOverVoltageProtectionValue : public IMessage {
    public:
        MessageSetOverVoltageProtectionValue(TChannel channel, double voltage) {
            mChannel = channel;
            mVoltage = voltage;
        }

        QByteArray query() const override {
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
    class MessageGetOverVoltageProtectionValue : public IMessage {
    public:
        MessageGetOverVoltageProtectionValue(TChannel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("OVPSET%1?").arg(mChannel).toLatin1();
        }

        int answerLength() const override {
            return 5;
        };

        bool allowDrop() const override {
            return true;
        }
    };
}





#endif //PSC_PROTOCOL_H
