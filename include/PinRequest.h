#pragma once
#include "Request.h"

using namespace request;

class PinRequest : public Request
{
private:
public:
    int pin;
    String mode;
    String signal;
    int digital;
    int analog;

    PinRequest(String &source) : Request(source)
    {
        if (isJSON)
        {
#if DEBUG_REQUEST
            Serial.printf("JSON:\n%s\n", body.c_str());
#endif
            parseJSON();
        }
        else
        {
            parseCommands();
        }
    }

    void parseJSON()
    {
        pin = doc["pin"];
        mode = (char const *)doc["mode"];
        signal = (char const *)doc["signal"];
        digital = doc["digital"];
        analog = doc["analog"];
    }

    void parseCommands()
    {
        Pair cmd;
        for (size_t i = 0; i < size; i++)
        {
            cmd = commands[i];
            if (cmd.name == "pin")
            {
                pin = cmd.value.toInt();
            }
            else if (cmd.name == "mode")
            {
                mode = cmd.value;
            }
            else if (cmd.name == "signal")
            {
                signal = cmd.value;
            }
            else if (cmd.name == "digital")
            {
                digital = cmd.value.toInt();
            }
            else if (cmd.name == "analog")
            {
                analog = cmd.value.toInt();
            }
        }
    }

    ~PinRequest()
    {
    }
};
