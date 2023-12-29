# PlatformIO ESP 8266 Motorized Gate Wifi Automation

## Overview for the project

The project is the automation code made for PlatformIO IDE and arduino based code.
Uses the pinout from the gate motor to trigger the wifi automation.
It brings the commands from the motor to a mqtt server.
Also allows led strips to be commanded with mqtt commands.

![Automation box image](https://github.com/tinel-c/PlatformIO_ESP8266_Second_gate_automation/blob/main/img/Gate_automation.png?raw=true)

## Schematic overview of the project

![Automation box image](https://github.com/tinel-c/PlatformIO_ESP8266_Second_gate_automation/blob/main/img/Second_gate_automation_schematic.PNG?raw=true)

### HW used in the project

* 12V Power source 60W to power the led strips
* ESP8266 4 relay board to automate the gate motor
* 2 FR120n mosfet modules to power the led strips

## Configuration

Take password.h_template and rename to password.h as a file.
Configure the network and ip for the mqtt server inside the file.
Compile and upload to the board.

## MQTT command structure

* Commands accepted

```
MainGate/CMD/Relay1               ON / OFF
MainGate/CMD/Relay2               ON / OFF
MainGate/CMD/Relay3               ON / OFF
MainGate/CMD/Relay4               ON / OFF

```

* Status reports

```
MainGate/STAT/Relay1              ON / OFF
MainGate/STAT/Relay2              ON / OFF
MainGate/STAT/Relay3              ON / OFF
MainGate/STAT/Relay4              ON / OFF

MainGate/STAT/eventRelay          ON / OFF
MainGate/STAT/eventKeypad         ON / OFF
MainGate/STAT/eventPower          ON / OFF

```

```
MainGate/STAT/reccurentStatusRelay1  Relay 1 OFF / Relay 1 ON
MainGate/STAT/reccurentStatusRelay2  Relay 2 OFF / Relay 2 ON
MainGate/STAT/reccurentStatusRelay3  Relay 3 OFF / Relay 3 ON
MainGate/STAT/reccurentStatusRelay4  Relay 4 OFF / Relay 4 ON
MainGate/STAT/reccurentStatusKeypad  Keypad OFF / Keypad ON
MainGate/STAT/reccurentStatusMains   Mains OFF / Mains ON

```
* Debug messages

```
MainGate/STAT/message
```

## Node-RED command structure

![Automation box image](https://github.com/tinel-c/PlatformIO_ESP8266_Second_gate_automation/blob/main/img/Node_red_automation.PNG?raw=true)