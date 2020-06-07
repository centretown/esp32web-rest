# ESP32 - Pin Controller
A simple web/rest server and client for the ESP32 micro controller.
# VSCode
[Install here.](http://code.visualstudio.com/download)
# Platformio
[Install here.](https://platformio.org/install/ide?install=vscode)
# Build and Deploy
From **VSCode** select **Platformio** then from **Project Tasks** select **Upload and Monitor** which automatically builds the project if required.
# Web API:
  - command:
    `/`
  - actions:
    `showWebPage();`
  - sends to client:
    `*the html string defined in tabs.h*`

  - command:
	`/api/?pin=2&mode=input&signal=digital`
  - actions:
    `pinMode(2, INPUT);`
    `digital = digitalRead(2);`
  - sends JSON to client:
    `{"pin":2,"mode":"input","signal":"digital","digital":digital,"analog":0}`

  - command:
	`/api/?pin=2&mode=output&signal=digital&digital=1`
  - actions:
    `pinMode(2, OUTPUT);`
    `digitalWrite(2, HIGH);`
  - sends JSON to client:
    `{"pin":2,"mode":"output","signal":"digital","digital":1,"analog":0}`

  - command:
  	`/api/?pin=6&mode=input&signal=analog`
  - actions:
    `pinMode(6, INPUT);`
    `analog = analogRead(6);`
  - sends JSON to client:
    `{"pin":6,"mode":"input","signal":"analog","digital":1,"analog":analog}`
