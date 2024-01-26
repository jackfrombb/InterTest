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

    static bool err(String sender, esp_err_t val)
    {
        switch (val)
        {
        case ESP_OK:
            // p(sender, "Init OK");
            return true;
            break;

        case ESP_FAIL:
            err(sender, "Init ERROR");
            break;

        case ESP_ERR_NO_MEM:
            err(sender, "Out of memory");
            break;

        case ESP_ERR_INVALID_ARG:
            err(sender, "Invalid argument");
            break;

        case ESP_ERR_INVALID_STATE:
            err(sender, "Invalid state");
            break;

        case ESP_ERR_INVALID_SIZE:
            err(sender, "Invalid size");
            break;

        case ESP_ERR_NOT_FOUND:
            err(sender, "Requested resource not found");
            break;

        case ESP_ERR_NOT_SUPPORTED:
            err(sender, "Operation or feature not supported");
            break;

        case ESP_ERR_TIMEOUT:
            err(sender, "Operation timed out");
            break;

        case ESP_ERR_INVALID_RESPONSE:
            err(sender, "Received response was invalid");
            break;

        case ESP_ERR_INVALID_CRC:
            err(sender, " CRC or checksum was invalid");
            break;

        case ESP_ERR_INVALID_VERSION:
            err(sender, "Version was invalid");
            break;

        case ESP_ERR_INVALID_MAC:
            err(sender, "MAC address was invalid");
            break;

        case ESP_ERR_NOT_FINISHED:
            err(sender, "There are items remained to retrieve");
            break;

            // WIFI errs==============================================

        case ESP_ERR_WIFI_BASE:
            err(sender, "Starting number of WiFi error codes");
            break;

        case ESP_ERR_MESH_BASE:
            err(sender, "Starting number of MESH error codes");
            break;

        case ESP_ERR_FLASH_BASE:
            err(sender, "Starting number of flash error codes");
            break;

        case ESP_ERR_HW_CRYPTO_BASE:
            err(sender, "Starting number of HW cryptography module error codes");
            break;

        case ESP_ERR_MEMPROT_BASE:
            err(sender, "Starting number of Memory Protection API error codes");
            break;
        }

        return false;
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