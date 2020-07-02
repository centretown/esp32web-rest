/*
    request.h - a simple request parser
*/

#pragma once
//#include <Arduino.h>
#include <ArduinoJson.h>

namespace request
{
    struct Pair
    {
        String name;
        String value;
    };

    class Request
    {
    private:
        const char *content_length = "Content-Length: ";

    public:
        enum
        {
            METHOD,
            PATH,
            VERSION,
            MAX,
            MAX_COMMANDS = 8
        };

        Pair commands[MAX_COMMANDS];
        String method;
        String path;
        String version;
        String body;
        size_t size = 0;
        bool isJSON = false;
        StaticJsonDocument<256> doc;

        Request(String &source)
        {
            parse(source);
        }

        ~Request()
        {
        }

        void parse(String &source)
        {
            char m[8], p[256], v[16];
            sscanf(source.c_str(), "%s %s %s\n", m, p, v);
#ifdef DEBUG_REQUEST
            Serial.printf("n: %d, m: %s, p: %s, v: %s\n", n, m, p, v);
#endif
            method = m;
            path = p;
            version = v;
            if (method == "GET")
            { // handle gets in path
                int begin = path.indexOf("?");
                if (begin >= 0)
                {
                    body = path.substring(begin);
                }
            }
            else if (method == "POST")
            { // handle posts in content
                int end = -1;
                int begin = source.indexOf(content_length);
                if (begin >= 0)
                {
                    end = source.indexOf("\n", begin);
                    if (end >= 0)
                    {
                        int clen = source.substring(begin + strlen(content_length), end).toInt();
                        int len = source.length();
                        body = source.substring(len - clen, len);
#ifdef DEBUG_REQUEST
                        Serial.print("content-length: ");
                        Serial.println(clen);
#endif
                    }
                }
            }
#ifdef DEBUG_REQUEST
            Serial.print("body:'");
            Serial.print(body);
            Serial.print("';'");
#endif
            // scan the body for parameters
            isJSON = body.charAt(0) == '{';

            if (isJSON)
            {
                DeserializationError error = deserializeJson(doc, body);
                // Test if parsing succeeds
                if (error)
                {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.c_str());
                    Serial.println(body);
                    return;
                }
            }
            else
            {
                int offset = 0;
                for (size = 0; offset >= 0 && size < MAX_COMMANDS; size++)
                {
                    offset = parsePair(offset, size);
#ifdef DEBUG_REQUEST
                    Serial.printf("i=%d, offset=%d, name=%s, value=%s\n",
                                  i, offset, commands[i].name.c_str(), commands[i].value.c_str());
#endif
                }
            }
        }

        int parsePair(int offset, int cmd)
        {
            String buffer;
            int ampPos = body.indexOf('&', offset);
            if (ampPos > offset)
            {
                buffer = body.substring(offset, ampPos);
            }
            else
            {
                buffer = body.substring(offset);
            }

            int eqPos = buffer.indexOf('=');
            if (eqPos > 0)
            {
                commands[cmd].name = buffer.substring(0, eqPos);
                commands[cmd].value = buffer.substring(eqPos + 1);
            }

            // skip past & to return next position
            if (ampPos >= 0)
            {
                ampPos++;
            }
            return ampPos;
        }
    };

} // namespace request
