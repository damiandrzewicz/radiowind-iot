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

// Radio operations
bool RadioWindCore::radioSetup(uint8_t nodeId, const RadioConfig &radioConfig)
{
    //Log.verboseln(F("Appliance::radioSetup"));

    Log.noticeln(F("RadioConfig(nodeId=%d, networkId=%d, rssi=%d, customFreq=%u, encryptKey=%s, gatewayId=%d"), 
        nodeId, radioConfig.networkId, radioConfig.rssi, radioConfig.customFrequency, radioConfig.encryptKey, radioConfig.gatewayId);

    if(radio_.initialize(RF69_868MHZ, nodeId, radioConfig.networkId))
    {
        if(radioConfig.customFrequency)
        {
            radio_.setFrequency(radioConfig.customFrequency);
        }

        radio_.setHighPower();
        radio_.enableAutoPower(radioConfig.rssi);
        radio_.encrypt(radioConfig.encryptKey);
        radio_.sleep();

        Log.noticeln(F("setup ok!"));

        //radioInitialized_ = true;

        return true;
    }
    else
    {
        Log.fatalln(F("setup failed!"));
        return false;
    }
}

bool RadioWindCore::radioSend(uint8_t gatewayId, bool ack)
{
    //Log.verboseln(F("Appliance::radioSend"));

    auto data = messageBuffer_.data();
    auto size = strlen(data);

    Log.noticeln(F("[RADIO OUT<<<<]: data=%s, size=%d, ack=%d, gatewayId=%d"), data, size, ack, gatewayId);

    if(ack)
    {
        if(radio_.sendWithRetry(gatewayId, data, size, 2, 100U))
        {
            Log.noticeln(F("send ok!"));
            if(radioPayloadToBuffer())
            {
                Log.noticeln(F("got payload into buffer!"));
                return true;
            }
            else
            {
                Log.warningln(F("missing response!"));
                return false;
            }
        }
        else
        {
            Log.warningln(F("radio sent FAILED!"));
            return false;
        }
    }
    else
    {
        radio_.send(gatewayId, data, size);
        return true;
    }
}

bool RadioWindCore::radioPayloadToBuffer()
{
    //Log.verboseln(F("Appliance::radioPayloadToBuffer"));

    if (radio_.DATALEN) 
    {
        auto data = reinterpret_cast<const char*>(radio_.DATA);
        auto size = strlen(data);
        
        if(radio_.DATALEN == size)  // got a valid packet?
        {
            messageBuffer_ = data;  // copy message to the buffer
            Log.noticeln(F("[RADIO IN>>>>]: data=%s, size=%d, senderId=%d, rssi=%d"), messageBuffer_.data(), size, radio_.SENDERID, radio_.RSSI);
            return true;
        }
    }

    return false;
}

void RadioWindCore::sendACKRepsonse(const MessageBuffer *request)
{
    //Log.verboseln(F("Appliance::sendACKRepsonse"));
    // Do not add logs before send ACK because of timing issues!

    if(request)
    {
        auto data = request->data();
        auto size = strlen(data);
        radio_.sendACK(data, size);
        Log.noticeln(F("[RADIO OUT(AKC)<<<<]: data=%s, size=%d, senderId=%d"), data, size, radio_.SENDERID);
    }
    else
    {
        radio_.sendACK();
        Log.noticeln(F("[RADIO OUT(AKC)<<<<]: EMPTY, senderId=%d"),  radio_.SENDERID);
    }
}

RadioConfig RadioWindCore::getRadioConfigForPairing()
{
    //Log.verboseln(F("Appliance::getRadioConfigForPairing"));

    RadioConfig radioConfigData;
    radioConfigData.gatewayId = 1;
    radioConfigData.networkId = 111;
    radioConfigData.customFrequency = 869000000L;
    strcpy_P(radioConfigData.encryptKey, PSTR("sampleEncryptKey"));
    radioConfigData.rssi = -80;

    return radioConfigData;
}