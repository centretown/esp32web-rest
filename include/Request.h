/*
    request.h - a simple http request parser
*/

#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>

namespace request
{
    struct Pair
    {
        String name;
        String value;
    }; // namespace requeststructPair

    class Request
    {
    private:
        const char *content_length = "Content-Length: ";
        char m[8], p[512], v[16];

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

        Request(String &source)
        {
            parse(source);
        }

        ~Request()
        {
        }

        void parse(String &source)
        {
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
            if (body.charAt(0) == '{')
            {
                // todo JSON
            }
            else
            {
                int offset = 0;
                for (int i = 0; offset >= 0 && i < MAX_COMMANDS; i++)
                {
                    offset = parsePair(offset, i);
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

    class PinRequest : public Request
    {
    private:
    public:
        int pin;
        String mode;
        String signal;
        int digital;
        int analog;

        PinRequest(String source) : Request(source)
        {
            Pair cmd;
            for (size_t i = 0; i < MAX_COMMANDS; i++)
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

} // namespace request

#endif // REQUEST_H