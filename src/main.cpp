/*
File: main.cpp
This example for the ESP32 WiFi is inspired from the Arduino example at:
http://www.arduino.org/learning/tutorials/boards-tutorials/restserver-and-restclient
Which demonstrates how you can create your own API using REST style.

Web API:
  see README.md
*/

//#define DEBUG_REQUEST

// Import required libraries
#include <Arduino.h>
#include <WiFi.h>
#include "Request.h"
// must define char *ssid and char *pass)
#include "credentials.h"
#include "tabs.h"

//using namespace request;

// Create an instance of the server
WiFiServer server(80);

void process(WiFiClient client);
void processApi(WiFiClient client, request::PinRequest req);
void ok(WiFiClient client);
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

  // Indicate the IP address
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

  String s = client.readString();
  //Serial.println(s);

  request::PinRequest req = request::PinRequest(s);
  //Serial.println(req.path.c_str());
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
  ok(client);
  showPin(client, pin, req.mode, req.signal, digital, analog);
}

void ok(WiFiClient client)
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  // indicate JSON per RFC 4627
  client.println("Content-type:application/json");
  client.println();
}

void showPin(WiFiClient client, int pin, String mode, String signal, int digital, int analog)
{
  char buffer[128];
  sprintf(buffer, "{\"pin\":%d,\"mode\":\"%s\",\"signal\":\"%s\",\"digital\":%d,\"analog\":%d}\n",
          pin, mode.c_str(), signal.c_str(), digital, analog);
  client.println(buffer);
  Serial.println(buffer);
}
