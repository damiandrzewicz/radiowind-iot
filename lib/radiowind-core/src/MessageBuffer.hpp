#pragma once

#include <Arduino.h>

class MessageBuffer
{
public:
    MessageBuffer()
    {
        clear();
    }

    MessageBuffer(const char *str)
    {
        clear();
        strncpy(buffer_, str, strlen(buffer_));
    }

    void appendText(const char *data, bool last = false)
    {
        strcat(buffer_, data);
        if(!last) appendDelimeter();
    }

    void appendInt(long value, bool last = false)
    {
        snprintf(buffer_ + strlen(buffer_), sizeof(buffer_) - strlen(buffer_), "%ld", value);
        if(!last) appendDelimeter();
    }

    void appendFloat(double value, uint8_t size = 4, uint8_t prec = 2, bool last = false)
    {
        char temp[10];
        dtostrf(value, size, prec, temp);
        strcat(buffer_, temp);
        if(!last) appendDelimeter();
    }

    void appendDelimeter()
    {
        strcat(buffer_, "|");
    }

    const char *getDelimeter() const
    {
        return "|";
    }

    bool isFilled()
    {
        return strlen(buffer_);
    }

    void clear()
    {
        memset(buffer_, 0, sizeof(buffer_));
    }

    char *data()
    {
        return buffer_;
    }

    const char *data() const
    {
        return buffer_;
    }

    MessageBuffer &operator=(const char *str)
    {
        clear();
        strncpy(buffer_, str, strlen(buffer_));
        return *this;
    }

private:
    char buffer_[100];
};