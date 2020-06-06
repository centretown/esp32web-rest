/*
File: main.cpp
This example for the Arduino Uno WiFi shows how to access the digital and analog pins
of the board through REST calls. It demonstrates how you can create your own API using
REST style.

Possible commands created in this sketch:

	"/arduino/digital/13"     -> digitalRead(13)
	"/arduino/digital/13/1"   -> digitalWrite(13, HIGH)
	"/arduino/analog/2/123"   -> analogWrite(2, 123)
	"/arduino/analog/2"       -> analogRead(2)
	"/arduino/mode/13/input"  -> pinMode(13, INPUT)
	"/arduino/mode/13/output" -> pinMode(13, OUTPUT)


http://www.arduino.org/learning/tutorials/boards-tutorials/restserver-and-restclient
*/

//#define DEBUG_REQUEST

// Import required libraries
#include <Arduino.h>
#include <WiFi.h>
#include "Request.h"
#include "credentials.h"
#include "tabs.h"

//using namespace request;

// Create an instance of the server
WiFiServer server(80);

void process(WiFiClient client);
void processApi(WiFiClient client, request::PinRequest req);

void modeCommand(WiFiClient client);
void digitalCommand(WiFiClient client);
void analogCommand(WiFiClient client);

void ok(WiFiClient client);
void err(WiFiClient client, const char *msg);

void showPin(WiFiClient client, int pin, String mode, String signal, int digital, int analog);
void showWebPage(WiFiClient client);

void setup()
{

  // Start Serial
  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop()
{
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  while (!client.available())
  {
    delay(1);
  }

  IPAddress ip = client.remoteIP(client.fd());
  Serial.println(ip.toString());
  process(client);
  client.stop();
}

void process(WiFiClient client)
{
  // read the command

  request::PinRequest req = request::PinRequest(client.readString());
  Serial.println(req.path.c_str());
  if (req.path.startsWith("/api/"))
  {
    processApi(client, req);
  }
  else
  {
    Serial.println("showWebPage");
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    // the content of the HTTP response follows the header:
    client.print(webpage);

    // The HTTP response ends with another blank line:
    client.println();
  }

  return;
}

void processApi(WiFiClient client, request::PinRequest req)
{

  Serial.print("Request mode=");
  Serial.print(req.mode);
  Serial.print(" type=");
  Serial.print(req.signal);
  Serial.print(" pin=");
  Serial.print(req.pin);
  Serial.print(" digital=");
  Serial.print(req.digital);
  Serial.print(" analog=");
  Serial.println(req.analog);

  char mode = req.mode.charAt(0);
  char signal = req.signal.charAt(0);
  int digital = req.digital;
  int analog = req.analog;
  int pin = req.pin;

  if (mode == 'i')
  {
    pinMode(pin, INPUT);
  }
  else if (mode == 'o')
  {
    pinMode(pin, OUTPUT);
  }

  if (signal == 'd')
  {
    if (mode == 'i')
    {
      digital = digitalRead(pin);
    }
    else if (mode == 'o')
    {
      digitalWrite(pin, digital);
    }
  }
  else if (signal == 'a')
  {
    analog = analogRead(pin);
  }
  char s[64];
  sprintf(s, "mode=%c, signal=%c, pin=%d, digital=%d, analog=%d\n",
          mode, signal, pin, digital, analog);
  Serial.println(s);
  /*
  Serial.print("mode=");
  Serial.print(mode);
  Serial.print(" signal=");
  Serial.print(signal);
  Serial.print(" pin=");
  Serial.print(pin);
  Serial.print(" digital=");
  Serial.print(digital);
  Serial.print(" analog=");
  Serial.println(analog);
*/
  ok(client);
  showPin(client, pin, req.mode, req.signal, digital, analog);
}

void err(WiFiClient client, const char *msg)
{
  ok(client);
  client.println(msg);
  client.println("");
}
 
void ok(WiFiClient client)
{
  client.println("HTTP/1.1 200 OK\n");
}

void showPin(WiFiClient client, int pin, String mode, String signal, int digital, int analog)
{
  char buffer[128];
  sprintf(buffer, "{\"pin\":%d,\"mode\":\"%s\",\"signal\":\"%s\",\"digital\":%d,\"analog\":%d}",
          pin, mode.c_str(), signal.c_str(), digital, analog);
  Serial.println(buffer); 
  client.println(buffer);
  /*
  String s = "{ \"pin\": ";
  s.concat(pin);
  s.concat(", \"mode\": \"");
  s.concat(mode);
  s.concat("\", \"signal\": \"");
  s.concat(signal);
  s.concat("\", \"digital\": ");
  s.concat(digital);
  s.concat(", \"analog\": ");
  s.concat(analog);
  s.concat("}");
  client.println(s);
  */
}
