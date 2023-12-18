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
SecondGate/CMD/Relay1               ON / OFF
SecondGate/CMD/Relay2               ON / OFF
SecondGate/CMD/Relay3               ON / OFF
SecondGate/CMD/Relay4               ON / OFF
SecondGate/CMD/Relay5               ON / OFF
SecondGate/CMD/Relay6               ON / OFF
```

* Status reports

```
SecondGate/STAT/Relay1              ON / OFF
SecondGate/STAT/Relay2              ON / OFF
SecondGate/STAT/Relay3              ON / OFF
SecondGate/STAT/Relay4              ON / OFF
SecondGate/STAT/Relay5              ON / OFF
SecondGate/STAT/Relay6              ON / OFF
```


* Debug messages

```
SecondGate/STAT/message
```

## Node-RED command structure

![Automation box image](https://github.com/tinel-c/PlatformIO_ESP8266_Second_gate_automation/blob/main/img/Node_red_automation.PNG?raw=true)