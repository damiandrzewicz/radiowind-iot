#pragma once

#include <Arduino.h>

struct UUIDConfig{
    byte uuidNumber[16];

    bool isEmpty() { return uuidNumber[0] == 0xff; }

    void setEmpty(){ uuidNumber[0] = 0xff; }

    void setDefaults()
    {
        memset(uuidNumber, 0, sizeof(uuidNumber));
    }

    UUIDConfig getDefaults()
    {
        UUIDConfig data;
        data.setDefaults();
        return data;
    }

    size_t size(){ return sizeof(UUIDConfig); }
};