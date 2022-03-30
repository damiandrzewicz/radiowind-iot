#include "RadioWindNode.hpp"

const char * const stateName[] PROGMEM = { 
    "ChekConfig",
    "RadioSend",
    "RadioReceive",
    "Sleep",
    "StateBtnDispatch",
    "RadioPair",
    "RadioReset",
    "FactoryReset",
};

void RadioWindNode::setup()
{
    Log.verboseln(F("s1"));
    RadioWindCore::setup();
    Log.verboseln(F("s2"));
}

void RadioWindNode::loop()
{
    RadioWindCore::loop();
}

void RadioWindNode::initStateMachine()
{
    // States
    stateMachine_.AddState(stateName[State::ChekConfig], 0, nullptr, [&](){ return onCheckConfig(); }, nullptr );
    stateMachine_.AddState(stateName[State::RadioSend], 0, nullptr, [&](){ return onRadioSend(); }, nullptr );
    stateMachine_.AddState(stateName[State::RadioReceive], 0, nullptr, [&](){ return onRadioReceive(); }, nullptr );
    stateMachine_.AddState(stateName[State::Sleep], 0, nullptr, [&](){ Log.verboseln("slp"); return onSleep(); }, nullptr );
    stateMachine_.AddState(stateName[State::StateBtnDispatch], 0, 
        [&](){ onEnterStateBtnDispatch(); }, 
        [&](){ return onStateBtnDispatch(); }, 
        [&](){ onLeaveStateBtnDispatch(); } 
    );
    stateMachine_.AddState(stateName[State::RadioPair], 0, nullptr, [&](){ return onRadioPair(); }, nullptr );
    stateMachine_.AddState(stateName[State::RadioReset], 0, nullptr, [&](){ return onRadioReset(); }, nullptr );
    stateMachine_.AddState(stateName[State::FactoryReset], 0, nullptr, [&](){ return onFactoryReset(); }, nullptr );

    // Transitions
    stateMachine_.AddTransition(State::ChekConfig, State::Sleep, [&](auto *cs){
        return cs->result == FSM_StateResult::Error;
    });

    stateMachine_.AddTransition(State::ChekConfig, State::RadioSend, [&](auto *cs){
        return cs->result == FSM_StateResult::Ok;
    });

    stateMachine_.AddTransition(State::RadioSend, State::RadioReceive, [&](auto *cs){
        return cs->result == FSM_StateResult::Ok;
    });

    stateMachine_.AddTransition(State::RadioSend, State::StateBtnDispatch, [&](auto *cs){
        return cs->result == FSM_StateResult::Error;
    });

    stateMachine_.AddTransition(State::RadioReceive, State::StateBtnDispatch, [&](auto *cs){
        return cs->result != FSM_StateResult::Working;
    });

    stateMachine_.AddTransition(State::StateBtnDispatch, State::RadioPair, [&](auto *cs){
        return cs->timeout && this->stateBtnPress_ == StateBtnPress::Short;
    });

    stateMachine_.AddTransition(State::StateBtnDispatch, State::RadioReset, [&](auto *cs){
        return cs->timeout && this->stateBtnPress_ == StateBtnPress::Medium;
    });

    stateMachine_.AddTransition(State::StateBtnDispatch, State::FactoryReset, [&](auto *cs){
        return cs->timeout && this->stateBtnPress_ == StateBtnPress::Long;
    });

    stateMachine_.AddTransition(State::StateBtnDispatch, State::Sleep, [&](auto *cs){
        return cs->timeout && this->stateBtn_.getState() == HIGH;
    });

    stateMachine_.AddTransition(State::RadioPair, State::ChekConfig, [&](auto *cs){
        return cs->result != FSM_StateResult::Working;
    });

    stateMachine_.AddTransition(State::RadioReset, State::ChekConfig, [&](auto *cs){
        return cs->result != FSM_StateResult::Working;
    });

    stateMachine_.AddTransition(State::FactoryReset, State::ChekConfig, [&](auto *cs){
        return cs->result != FSM_StateResult::Working;
    });

    stateMachine_.AddTransition(State::Sleep, State::ChekConfig, [&](auto *cs){
        return cs->result != FSM_StateResult::Working;
    });
}

// Action handlers
void RadioWindNode::onEnterStateBtnDispatch()
{
    stateMachine_.CurrentState()->maxTime = stateBtn_.getDebounceTime() + 5;
    stateLed_.blink(50, 950, 1000);
}

FSM_StateResult RadioWindNode::onStateBtnDispatch()
{
    //Log.verboseln(F("onStateBtnDispatch"));
    return FSM_StateResult::Working;
}

void RadioWindNode::onLeaveStateBtnDispatch()
{
    //Log.verboseln(F("onLeaveStateBtnDispatch"));
    stateLed_.cancel();
}

FSM_StateResult RadioWindNode::onCheckConfig()
{
    if(radioConfig_.isEmpty())
    {
        sleepTime_ = 0; // forever
        FSM_StateResult::Error;
    }
    else
    {
        FSM_StateResult::Ok;
    }
}

FSM_StateResult RadioWindNode::onRadioPair()
{
    //Log.verboseln(F("onRadioPair"));
    return FSM_StateResult::Ok;
}

FSM_StateResult RadioWindNode::onRadioReset()
{
    return FSM_StateResult::Ok;
}

FSM_StateResult RadioWindNode::onFactoryReset()
{   
    return FSM_StateResult::Ok;
}

FSM_StateResult RadioWindNode::onRadioSend()
{
    return FSM_StateResult::Ok;
}

FSM_StateResult RadioWindNode::onRadioReceive()
{
    return FSM_StateResult::Ok;
}

FSM_StateResult RadioWindNode::onSleep()
{
    deepSleepForWakeupOnInt(sleepTime_, extInterruptPin_, FALLING);
    return FSM_StateResult::Ok;
}

// Power saving operations
void RadioWindNode::deepSleepFor(unsigned int delay)
{
    Log.verboseln(F("deep sleep for: %d ms"), delay);

    preDeepSleep();
    LowPowerWrapper::DeepSleep(delay);
    postDeepSleep();
}

void RadioWindNode::deepSleepForWakeupOnInt(unsigned int delay, uint8_t pin, uint8_t mode)
{
    Log.verboseln(F("deep sleep for: %d, interrput(pin: %d, mode: %d)"), delay, pin, mode);

    preDeepSleep();
    attachInterrupt(digitalPinToInterrupt(pin), [](){}, mode);
    delay ? LowPowerWrapper::DeepSleep(delay) : LowPowerWrapper::DeepSleepForever();
    detachInterrupt(digitalPinToInterrupt(pin)); 
    postDeepSleep();
}

void RadioWindNode::preDeepSleep()
{
    //Log.verboseln(F("Node::preDeepSleep"));

    Serial.flush();
    
    //if(stateMachineModel_.context_.radioConfigured)
    //{
        //radio_.sleep();
    //}
}

void RadioWindNode::postDeepSleep()
{
    //Log.verboseln(F("Node::postDeepSleep"));
} 