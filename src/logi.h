#pragma once
#include <Arduino.h>

typedef enum
{
    INFO,
    ERR
} logi_msg_type;

class logi
{
protected:
    static logi *instance;
    static bool _printToSerialEnabled;

private:
    void _printToSerial(logi_msg_type type, String sender, String text)
    {
        switch (type)
        {
        case logi_msg_type::ERR:
            Serial.println("ERROR - " + sender + ": " + text);
            break;

        case logi_msg_type::INFO:
        default:
            Serial.println(sender + ": " + text);
            break;
        }
    }

    logi()
    {
        _printToSerialEnabled = true;
    }

    logi(bool printToSerial)
    {
        _printToSerialEnabled = printToSerial;
    }

public:
    static void settings(bool enablePrintToSerial)
    {
        _printToSerialEnabled = enablePrintToSerial;
    }

    static logi *get()
    {
        if (instance == nullptr)
        {
            instance = new logi(_printToSerialEnabled);
        }
        return instance;
    }

    static void err(String sender, String text)
    {
        if (logi::get()->_printToSerialEnabled)
        {
            logi::get()->_printToSerial(logi_msg_type::ERR, sender, text);
        }
    }

    static void p(String sender, String text)
    {
        if (logi::get()->_printToSerialEnabled)
        {
            logi::get()->_printToSerial(logi_msg_type::INFO, sender, text);
        }
    }
};

logi *logi::instance = 0;
bool logi::_printToSerialEnabled = true;