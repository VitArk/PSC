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
// Created by Vitalii Arkusha on 17.09.2022.
//

#ifndef PSC_PROTOCOL_H
#define PSC_PROTOCOL_H

#include <QString>
#include "Global.h"

namespace Protocol {
    class IMessage {
    public:
        virtual ~IMessage() = default;

        virtual Global::Channel channel() const { return mChannel;}
        virtual QByteArray query() const = 0;
        virtual int replySize() const { return 0; }
        virtual bool isCommandWithReply() const { return replySize() > 0; }
        // messages that return true, will be dropped in case overflowing messages queue.
        virtual bool allowToDrop() const { return false; }
    protected:
        Global::Channel mChannel = Global::Channel1;
    };

    /**
     * LOCK<NR2>
     * Function Description:Lock power supply operation panel
     * Example: LOCK1
     * MessageSetLocked power supply operation panel
     * Example:LOCK0
     * Unlock power supply operation panel
     */
    class MessageSetLocked : public IMessage {
    public:
        explicit MessageSetLocked(bool lock) {
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
    class MessageGetIsLocked : public IMessage {
    public:
        QByteArray query() const override {
            return "LOCK?";
        }

        int replySize() const override {
            return 1;
        };

        bool allowToDrop() const override {
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
        MessageSetCurrent(Global::Channel channel, double current) {
            mChannel = channel;
            mCurrent = current;
        }

        QByteArray query() const override {
            return QString("ISET%1:%2").arg(mChannel).arg(QString::asprintf("%05.03f", mCurrent)).toLatin1();
        }

    private:
        double   mCurrent;
    };

    /**
     * ISET<X>?
     * Function Description: Get current that has been set
     * Example: ISET1?
     * Returns current value
     */
    class MessageGetCurrentSet : public IMessage {
    public:
        explicit MessageGetCurrentSet(Global::Channel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("ISET%1?").arg(mChannel).toLatin1();
        }

        int replySize() const override {
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
        explicit MessageSetVoltage(Global::Channel channel, double voltage) {
            mChannel = channel;
            mVoltage = voltage;
        }

        QByteArray query() const override {
            return QString("VSET%1:%2").arg(mChannel).arg(QString::asprintf("%05.02f", mVoltage)).toLatin1();
        }

    private:
        double   mVoltage;
    };

    /**
     * VSET<X>?
     * Function Description: Get voltage that has been set
     * Example: VSET1?
     * Returns voltage value
     */
    class MessageGetVoltageSet : public IMessage {
    public:
        explicit MessageGetVoltageSet(Global::Channel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("VSET%1?").arg(mChannel).toLatin1();
        }

        int replySize() const override {
            return 5;
        };
    };

    /**
     * IOUT<X>?
     * Function Description: Read current SetEnableOutputSwitch value
     * Example: IOUT1?
     * Read the set current value
     */
    class MessageGetActualCurrent : public IMessage {
    public:
        explicit MessageGetActualCurrent(Global::Channel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("IOUT%1?").arg(mChannel).toLatin1();
        }

        int replySize() const override {
            return 5;
        };
    };

    /**
    * VOUT<X>?
    * Function Description: Read voltage SetEnableOutputSwitch value
    * Example: VOUT1?
    * Read the set voltage value
    */
    class MessageGetActualVoltage : public IMessage {
    public:
        explicit MessageGetActualVoltage(Global::Channel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("VOUT%1?").arg(mChannel).toLatin1();
        }

        int replySize() const override {
            return 5;
        };
    };

    /**
    * OUT<Boolean>
    * Function Description: Turn on/off power supply SetEnableOutputSwitch
    * Boolean: 0 off; 1 on
    * Example: OUT1 Turn on power supply SetEnableOutputSwitch
    */
    class MessageSetEnableOutputSwitch : public IMessage {
    public:
        explicit MessageSetEnableOutputSwitch(bool state) {
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
    * Function Description: Turn on/off SetEnableBeep
    * Example: BEEP1 Turn on SetEnableBeep
    */
    class MessageSetEnableBeep : public IMessage {
    public:
        explicit MessageSetEnableBeep(bool state) {
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
    class MessageGetIsBeepEnabled : public IMessage {
    public:
        QByteArray query() const override {
            return "BEEP?";
        }
        int replySize() const override {
            return 1;
        };
        bool allowToDrop() const override {
            return true;
        }
    };

    /**
     * STATUS?
     * Function Description: Read power supply SetEnableOutputSwitch status
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
        int replySize() const override {
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

        int replySize() const override {
            return 9;
        };
    };

    /**
     * RCL<NR1>
     * Function Description:Storage recall by pressing keys from M1-M5
     */
    class MessageSetPreset : public IMessage {
    public:
        explicit MessageSetPreset(Global::MemoryKey key) {
            mKey = key;
        }

        QByteArray query() const override {
            return QString("RCL%1").arg(mKey).toLatin1();
        }
    private:
        Global::MemoryKey mKey;
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
        int replySize() const override {
            return 1;
        };
        bool allowToDrop() const override {
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
        explicit MessageSavePreset(Global::MemoryKey key) {
            mKey = key;
        }

        QByteArray query() const override {
            return QString("SAV%1").arg(mKey).toLatin1();
        }
    private:
        Global::MemoryKey mKey;
    };

    /**
     * TRACK<NR1>
     * Function Description: Set series & parallel channels Tracking
     * NR1: 0=independent output; 1=series output; 2=parallel
     * Example: TRACK1
     */
    class MessageSetChannelTracking : public IMessage {
    public:
        explicit MessageSetChannelTracking(Global::ChannelsTracking mode) {
            mMode = mode;
        }

        QByteArray query() const override {
            return QString("TRACK%1").arg(mMode).toLatin1();
        }
    private:
        Global::ChannelsTracking mMode;
    };

    /**
     * OCP<Boolean>
     * Function Description: Turn on over current protection
     * Boolean: 0 OFF, 1 ON
     * Example: OCP1 Turn on OCP
     */
    class MessageSetEnableOverCurrentProtection : public IMessage {
    public:
        explicit MessageSetEnableOverCurrentProtection(bool state) {
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
    class MessageSetEnableOverVoltageProtection : public IMessage {
    public:
        explicit MessageSetEnableOverVoltageProtection(bool state) {
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
        explicit MessageSetOverCurrentProtectionValue(Global::Channel channel, double value) {
            mChannel = channel;
            mValue = value;
        }

        QByteArray query() const override {
            return QString("OCPSET%1:%2").arg(mChannel).arg(QString::asprintf("%05.03f", mValue)).toLatin1();
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
        explicit MessageGetOverCurrentProtectionValue(Global::Channel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("OCPSET%1?").arg(mChannel).toLatin1();
        }

        int replySize() const override {
            return 5;
        };

        bool allowToDrop() const override {
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
        explicit MessageSetOverVoltageProtectionValue(Global::Channel channel, double voltage) {
            mChannel = channel;
            mVoltage = voltage;
        }

        QByteArray query() const override {
            return QString("OVPSET%1:%2").arg(mChannel).arg(QString::asprintf("%05.02f", mVoltage)).toLatin1();
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
        explicit MessageGetOverVoltageProtectionValue(Global::Channel channel) {
            mChannel = channel;
        }

        QByteArray query() const override {
            return QString("OVPSET%1?").arg(mChannel).toLatin1();
        }

        int replySize() const override {
            return 5;
        };

        bool allowToDrop() const override {
            return true;
        }
    };
}

#endif //PSC_PROTOCOL_H
