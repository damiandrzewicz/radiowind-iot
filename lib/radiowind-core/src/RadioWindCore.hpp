#pragma once

#include <Arduino.h>
#include <ArduinoLog.h>
#include "RadioConfig.hpp"
#include "RFM69_ATC.h"
#include <ezButton.h>
#include <ezLED.h>
#include <EEPROM.h>
#include "YA_FSM.h"

class RadioWindCore
{
public:
    enum class StateBtnPress
    {
        NoPress,
        Single,
        Short,
        Medium,
        Long
    };

    RadioWindCore(uint8_t stateBtnPin, uint8_t stateLedPin, int8_t extInterruptPin = -1);
    RadioWindCore(uint8_t nssPin, uint8_t irqPin, uint8_t stateBtnPin, uint8_t stateLedPin, int8_t extInterruptPin = -1);

    ~RadioWindCore() = default;

    // Important functions
    virtual void setup();
    virtual void loop();

protected:
    virtual void initStateMachine() = 0;

    // Helper functions
    StateBtnPress stateBtnCheck();

    // Radio functions

protected:
    RFM69_ATC radio_;
    RadioConfig radioConfig_;

    unsigned long sleepTime_ = 0;

    ezButton stateBtn_;
    StateBtnPress stateBtnPress_ = StateBtnPress::NoPress;
    ezLED stateLed_;
    int8_t extInterruptPin_;

    YA_FSM stateMachine_;
};
