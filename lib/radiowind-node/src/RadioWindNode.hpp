#pragma once

#include "RadioWindCore.hpp"
#include <ArduinoLog.h>
#include "utils/LowPowerWrapper.hpp"
#include "YA_FSM.h"

class RadioWindNode : public RadioWindCore
{
public:
    enum State {
        ChekConfig,
        RadioSend,
        RadioReceive,
        Sleep,
        StateBtnDispatch,
        RadioPair,
        RadioReset,
        FactoryReset,
    };

    RadioWindNode() : RadioWindCore(4, 9, 3){}

    virtual void setup() override;
    virtual void loop() override;

protected:

    virtual void initStateMachine() override;

    // Action handlers
    void onEnterStateBtnDispatch();
    FSM_StateResult onStateBtnDispatch();
    void onLeaveStateBtnDispatch();
    FSM_StateResult onCheckConfig();
    FSM_StateResult onRadioPair();
    FSM_StateResult onRadioReset();
    FSM_StateResult onFactoryReset();
    FSM_StateResult onRadioSend();
    FSM_StateResult onRadioReceive();
    FSM_StateResult onSleep();

    // Power saving operations
    void deepSleepFor(unsigned int delay);
    void deepSleepForWakeupOnInt(unsigned int delay, uint8_t pin, uint8_t mode);
    void preDeepSleep();
    void postDeepSleep();

private:
    
};

