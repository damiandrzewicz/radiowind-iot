#include "RadioWindCore.hpp"

RadioWindCore::RadioWindCore(uint8_t stateBtnPin, uint8_t stateLedPin, int8_t extInterruptPin = -1)
    : stateBtn_(stateBtnPin), stateLed_(stateLedPin), extInterruptPin_(extInterruptPin)
{

}

RadioWindCore::RadioWindCore(uint8_t nssPin, uint8_t irqPin, uint8_t stateBtnPin, uint8_t stateLedPin, int8_t extInterruptPin = -1)
    : RadioWindCore(stateBtnPin, stateLedPin, extInterruptPin)
{
    radio_ = { nssPin, irqPin };
} 

void RadioWindCore::setup()
{
    stateBtn_.setDebounceTime(40);  // Set debounce time for buttons
    EEPROM.get(0, radioConfig_);    // Read radio config from EEPROM

    initStateMachine();             // Initialize state machine
}

void RadioWindCore::loop()
{
    // StateBtn loop
    stateBtn_.loop();
    stateBtnPress_ = stateBtnCheck();

    // StateLed loop
    stateLed_.loop();

    // StateMachine loop
    stateMachine_.Update();
}

RadioWindCore::StateBtnPress RadioWindCore::stateBtnCheck()
{
    //Log.verboseln("stateBtnCheck");
    static constexpr uint16_t shortPressTime = 3000;
    static constexpr uint16_t mediumPressTime = 10000;
    static constexpr uint16_t longPressTime = 15000;

    static unsigned long pressedTime = 0;
    static unsigned long releasedTime = 0;

    StateBtnPress press = StateBtnPress::NoPress;

    if(stateBtn_.isPressed()){
        pressedTime = millis();
        Log.verboseln(F("pressed: %d"), pressedTime);
    }
    
    if(stateBtn_.isReleased()) {
        releasedTime = millis();
        Log.verboseln(F("released: %d"), releasedTime);

        press = StateBtnPress::Single;
        long pressDuration = releasedTime - pressedTime;

        if( pressDuration >= shortPressTime ){
            press = StateBtnPress::Short;
        }

        if( (pressDuration >= mediumPressTime) && (pressDuration < longPressTime) ){
            press = StateBtnPress::Medium;
        }

        if( pressDuration >= longPressTime ){
            press = StateBtnPress::Long;
        }
    }
    //Log.verboseln(F("btnMode: %d, pressedTime: %d, releasedTime: %d"), 
    //   static_cast<uint8_t>(mode), pressedTime, releasedTime);

    //Log.verboseln(F("state: %d"), static_cast<int>(state));

    return press;
}