#pragma once

#include <Arduino.h>

struct RadioConfig{
    uint8_t networkId;
    uint8_t gatewayId;
    unsigned long customFrequency = 0;
    char encryptKey[17];
    int8_t rssi = -80;

    bool isEmpty() { return networkId == 0xff; }

    void setEmpty(){ networkId = 0xff; }

    void setDefaults()
    {
        auto defs = getDefaults();
        gatewayId = defs.gatewayId;
        networkId = defs.networkId;
        customFrequency = defs.customFrequency;
        rssi = defs.rssi;
        strcpy(encryptKey, defs.encryptKey);
    }

    RadioConfig getDefaults()
    {
        RadioConfig data;
        data.gatewayId = 1;
        data.networkId = 100;
        data.customFrequency = 0;
        data.rssi = -80;
        strcpy_P(data.encryptKey, PSTR("sampleEncryptKey"));

        return data;
    }

    size_t size(){ return sizeof(RadioConfig); }
};