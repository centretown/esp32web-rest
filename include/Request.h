/*
    request.h - a simple http request parser
*/

#ifndef REQUEST_H
#define REQUEST_H

#include <Arduino.h>

namespace request
{
    struct ParseElement
    {
        int begin = -1;
        int end = -1;
        char term = ' ';
    };

    struct Pair
    {
        String name;
        String value;
    };

    /**
 * Parser - parses http request and provides access to it's elements
 **/
    class Parser
    {
    private:
        ParseElement *elements = NULL;

        const char *pre = "";
        char sep = ' ';
        char term = '\n';
        size_t max = 0;
        size_t size = 0;

    public:
        Parser(const char *prefix, char separator, char terminator, size_t maximum)
        {
            pre = prefix;
            sep = separator;
            term = terminator;
            max = maximum;
#ifdef DEBUG_REQUEST
            Serial.print("(prefix: ");
            Serial.print(pre);
            Serial.print(", separator: ");
            Serial.print((int)sep);
            Serial.print(", terminator:");
            Serial.print((int)term);
            Serial.print(", maximum:");
            Serial.print(maximum);
            Serial.println(")");
#endif
            if (max > 0)
            {
                elements = (ParseElement *)malloc(sizeof(ParseElement) * max);
                for (size_t i = 0; i < max - 1; i++)
                {
                    elements[i].term = sep;
                }
                elements[max - 1].term = term;
            }
        }

        ~Parser()
        {
            if (elements != NULL)
            {
                free(elements);
            }
        }

        String parse(String &source);

        String get(String &src, size_t index)
        {
            ParseElement *pEl = NULL;
            if (index >= max)
            {
                return String("index >= maximum");
            }
            pEl = &elements[index];

#ifdef DEBUG_REQUEST
            Serial.print("(begin: ");
            Serial.print(pEl->begin);
            Serial.print(", end:");
            Serial.print(pEl->end);
            Serial.println(")");
#endif
            if (pEl->begin < 0)
            {
                return String("begin < zero");
            }
            if (pEl->end <= pEl->begin)
            {
                return String("end <= begin");
            }
            return src.substring(pEl->begin, pEl->end);
        }

        size_t length()
        {
            return size;
        }
    };

    class RequestPath
    {
    private:
        enum
        {
            MAX = 16
        };

    public:
        Pair commands[MAX];
        size_t size;

        RequestPath(String &source)
        {
            Parser parser = Parser("/api/?", '&', '&', MAX);
            String req = parser.parse(source);
            size = parser.length();
#ifdef DEBUG_REQUEST
            Serial.print("length: ");
            Serial.println(size);
#endif
            Parser pairParser = Parser("", '=', ';', 2);
            String pairStr;
            for (size_t i = 0; i < size; i++)
            {
                pairStr = parser.get(req, i);
                pairParser.parse(pairStr);
                commands[i].name = pairParser.get(pairStr, 0);
                commands[i].value = pairParser.get(pairStr, 1);
#ifdef DEBUG_REQUEST
                Serial.println(commands[i].name.c_str());
                Serial.println(commands[i].value.c_str());
                Serial.println(pairStr.c_str());
#endif
            }
        }
        ~RequestPath()
        {
        }
    };

    class Request
    {
    private:
        enum
        {
            METHOD,
            PATH,
            VERSION,
            MAX,
            MAX_COMMANDS = 8
        };

        /* data */

    public:
        Pair commands[MAX_COMMANDS];
        String method;
        String path;
        String version;
        size_t size = 0;

        Request(String source)
        {
            Parser parser = Parser("", ' ', '\n', MAX);
            String req = parser.parse(source);

            method = parser.get(req, METHOD);
            path = parser.get(req, PATH);
            version = parser.get(req, VERSION);
        }
        ~Request()
        {
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
            RequestPath p = RequestPath(path);
            size = p.size;
            Pair cmd;
            for (size_t i = 0; i < size; i++)
            {
                cmd = p.commands[i];
                if (cmd.name == "pin")
                {
                    pin = cmd.value.toInt();
                    ;
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

    String Parser::parse(String &source)
    {
        String req = String("");
        // first line only
        int eol = source.indexOf('\n');
        if (eol < 0)
        {
            eol = source.length();
        }
#ifdef DEBUG_REQUEST
        Serial.print("eol: ");
        Serial.println(eol);
#endif
        if (eol > 0)
        {
            int index = 0;
            req = source.substring(0, eol);

            // skip prefix
            int l = strlen(pre);
            if (l > 0)
            {
                int px = req.indexOf(pre);
                if (px == 0)
                {
                    index += l;
                }
            }

            // skip separators
            while (req.charAt(index) == sep)
            {
                index++;
            }
#ifdef DEBUG_REQUEST
            Serial.println(req.c_str());
#endif
            int end = req.length();
            ParseElement *p;

            for (int i = 0; i < max; i++)
            {
                p = &elements[i];
                if (index < 0)
                {
                    p->begin = -1;
                    p->end = -1;
                }
                else
                {
                    p->begin = index;
                    p->end = req.indexOf(p->term, index);
                    if (p->end > 0)
                    {
                        index = p->end + 1;
                    }
                    else
                    {
                        p->end = end;
                        index = -1;
                        size = i + 1;
                    }
                }
#ifdef DEBUG_REQUEST
                Serial.print("i=");
                Serial.print(i);
                Serial.print(", index=");
                Serial.print(index);
                Serial.print(", end=");
                Serial.print(p->end);
                Serial.print(", term=");
                Serial.println((int)p->term);
#endif
            }
        }
#ifdef DEBUG_REQUEST
        Serial.print("length: ");
        Serial.println(size);
#endif
        return req;
    }
} // namespace request

#endif // REQUEST_H