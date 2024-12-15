#include "Arduino.h"
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <arduino-timer.h>
#include "password.h"


// internal relay status
char relayStatusArray[4] = {0,0,0,0};

// input status
char inputStatusArray[2] = {0,0};

// used relay to open the hate
char gateRelayUsed = 2;


// mqtt setup
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

// the interval that the system reports current status of the system
int statusReportReccurence = 30000;

// the interval for opening the relays
int relayGateOpenProcessingTime = 600;

// the interval when sample is taken for debounce
int debounceSampleTime = 1000;

// timer for status reporting
auto timer = timer_create_default();
auto relayProcessTimer = timer_create_default();
auto debounceTimer = timer_create_default();

//configure pins
int inputPowerAvailabilityPin = 5;
int inputKeypadPin = 4;

// input debounce create values for the interrupt that are treated on a 
// background task
int keyPadActivated = 0;
int keyPadDectivated = 0;

int mainsPowerDownActivated = 0;
int mainsPowerDownDeactivated = 0;


// activate the processing of the internal status of the relays
void processRelayStatus()
{
  // relay processing place the pin high / low depending on the mqtt request
	if(relayStatusArray[0] == 0)  { digitalWrite(16,LOW);} else { digitalWrite(16,HIGH);}
	if(relayStatusArray[1] == 0)  { digitalWrite(14,LOW);} else { digitalWrite(14,HIGH);}
	if(relayStatusArray[2] == 0)  { digitalWrite(12,LOW);} else { digitalWrite(12,HIGH);}
	if(relayStatusArray[3] == 0)  { digitalWrite(13,LOW);} else { digitalWrite(13,HIGH);}
}

//process actual status of the digital pins
void processInputPins()
{
  if(digitalRead(inputKeypadPin) == 0) {
    inputStatusArray[0] = 0;
  } else {
    inputStatusArray[0] = 1;
  }

  if(digitalRead(inputPowerAvailabilityPin) == 0) {
    inputStatusArray[1] = 0;
  } else {
    inputStatusArray[1] = 1;
  }

}

// close the relays after the keypad has been activated
bool closeRelay(void *) {
  char statusReport[20];
  Serial.println("Close relay 2!!!");
  strcpy(statusReport, "OFF");
  client.publish("MainGate/STAT/eventRelay", statusReport);
  client.publish("MainGate/STAT/Relay2", statusReport);
  relayStatusArray[gateRelayUsed] = 0;
  processRelayStatus();
  return false; // to repeat the action - false to stop
}

void openGate() {
  char statusReport[20];
  // open gate
  relayStatusArray[gateRelayUsed] = 1;
  // trigger relay back to 0 in 1 second
  processRelayStatus();
  Serial.println("Close relay 3!!!");
  strcpy(statusReport, "ON");
  client.publish("MainGate/STAT/eventRelay", statusReport);
  client.publish("MainGate/STAT/Relay3", statusReport);
  relayProcessTimer.in(relayGateOpenProcessingTime, closeRelay);
}

// function that treats the debouncing of the inputs
bool inputDebounceProcessing(void *) {
  char statusReport[20];
  // check what is the real output for the power
  processInputPins();
  //KeyPad availbility processing
  if ((keyPadActivated == 1) || (keyPadDectivated == 1)) {
    //eliminate the jittering of the signal
    if(inputStatusArray[0] == 1) {
      keyPadActivated = 1;
      keyPadDectivated = 0;
    } else {
      keyPadActivated = 0;
      keyPadDectivated = 1;
    } 

    if(keyPadActivated == 1) {
      Serial.println("Keypad activated!!!");
      strcpy(statusReport, "ON");
      client.publish("MainGate/STAT/eventKeypad", statusReport);
      openGate();
      keyPadActivated = 0;
    } 

    if(keyPadDectivated == 1) {
      Serial.println("Keypad deactivated!!!");
      strcpy(statusReport, "OFF");
      client.publish("MainGate/STAT/eventKeypad", statusReport);
      keyPadDectivated = 0;
    }
  }

  // process the mains interrupt
  if ((mainsPowerDownActivated == 1) || (mainsPowerDownDeactivated == 1)){
    //eliminate the jittering of the signal
    if(inputStatusArray[1] == 0) {
      mainsPowerDownActivated = 1;
      mainsPowerDownDeactivated = 0;
    } else {
      mainsPowerDownActivated = 0;
      mainsPowerDownDeactivated = 1;
    } 

    if(mainsPowerDownActivated == 1) {
      Serial.println("Power down detected!!!");
      strcpy(statusReport, "OFF");
      client.publish("MainGate/STAT/eventPower", statusReport);
      mainsPowerDownActivated = 0;
    }
    if (mainsPowerDownDeactivated == 1)  {
    Serial.println("Power recovered detected!!!");
    strcpy(statusReport, "ON");
    client.publish("MainGate/STAT/eventPower", statusReport);
    mainsPowerDownDeactivated = 0;
    } 
  } 
  
  return false;
}

// status function that reports every statusReportReccurence on mqtt the internal status
bool mqttStatusReporting(void *) {
  char statusReport[20];
  processInputPins();
  Serial.println("Status reporting");
  if(relayStatusArray[0] == 0) {
    strcpy(statusReport, "Relay 1 OFF");
    client.publish("MainGate/STAT/reccurentStatusRelay1", statusReport);
    Serial.println("Relay 1 OFF");
  } else {
    strcpy(statusReport, "Relay 1 ON");
    client.publish("MainGate/STAT/reccurentStatusRelay1", statusReport);
    Serial.println("Relay 1 ON");
  }

  if(relayStatusArray[1] == 0) {
    strcpy(statusReport, "Relay 2 OFF");
    client.publish("MainGate/STAT/reccurentStatusRelay2", statusReport);
    Serial.println("Relay 2 OFF");
  } else {
    strcpy(statusReport, "Relay 2 ON");
    client.publish("MainGate/STAT/reccurentStatusRelay2", statusReport);
    Serial.println("Relay 2 ON");
  }

  if(relayStatusArray[2] == 0) {
    strcpy(statusReport, "Relay 3 OFF");
    client.publish("MainGate/STAT/reccurentStatusRelay3", statusReport);
    Serial.println("Relay 3 OFF");
  } else {
    strcpy(statusReport, "Relay 3 ON");
    client.publish("MainGate/STAT/reccurentStatusRelay3", statusReport);
    Serial.println("Relay 3 ON");
  }

  if(relayStatusArray[3] == 0) {
    strcpy(statusReport, "Relay 4 OFF");
    client.publish("MainGate/STAT/reccurentStatusRelay4", statusReport);
    Serial.println("Relay 4 OFF");
  } else {
    strcpy(statusReport, "Relay 4 ON");
    client.publish("MainGate/STAT/reccurentStatusRelay4", statusReport);
    Serial.println("Relay 4 ON");
  }

  if(inputStatusArray[0] == 0) {
    strcpy(statusReport, "Keypad OFF");
    client.publish("MainGate/STAT/reccurentStatusKeypad", statusReport);
    Serial.println("Keypad OFF");
  } else {
    strcpy(statusReport, "Keypad ON");
    client.publish("MainGate/STAT/reccurentStatusKeypad", statusReport);
    Serial.println("Keypad ON");
  }
  if(inputStatusArray[1] == 0) {
    strcpy(statusReport, "Mains OFF");
    client.publish("MainGate/STAT/reccurentStatusMains", statusReport);
    Serial.println("Mains OFF");
  } else {
    strcpy(statusReport, "Mains ON");
    client.publish("MainGate/STAT/reccurentStatusMains", statusReport);
    Serial.println("Mains ON");
  }
  return true; // to repeat the action - false to stop
}

// correct key or fub activated
IRAM_ATTR void inputKeyPadInterrupt() {
  processInputPins();
  //KeyPad availbility processing
  if(inputStatusArray[0] == 1) {
    keyPadActivated = 1;
  } else {
    keyPadDectivated = 1;
  }
  debounceTimer.in(debounceSampleTime, inputDebounceProcessing);
}

// mains power recovered - system back on mains
IRAM_ATTR void inputPowerInterruptDetected() {
  processInputPins();
  if(inputStatusArray[1] == 0) {
    mainsPowerDownActivated = 1;
  } else {
    mainsPowerDownDeactivated = 1;
  } 
  debounceTimer.in(debounceSampleTime, inputDebounceProcessing);
}

void initRelayPins()
{
	// 220V Relays IO16, IO14, IO12, IO13
	pinMode(16,OUTPUT); 
	pinMode(14,OUTPUT);
	pinMode(12,OUTPUT);
	pinMode(13,OUTPUT);
  // initialize the pin status
  processRelayStatus();
}

void initInputPins()
{
  // input pins 
  // pin linked to the keypad input
	pinMode(inputKeypadPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(inputKeypadPin), inputKeyPadInterrupt, CHANGE);

  // pin linked to the power availability
	pinMode(inputPowerAvailabilityPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(inputPowerAvailabilityPin), inputPowerInterruptDetected, CHANGE);
  processInputPins();
}


void printToLCD(char* message) {
	client.publish("MainGate/STAT/message", message);
}

void printRuntimeToLCD(char* message) {
	client.publish("MainGate/STAT/message", message);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  printToLCD("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  printToLCD("WiFi connected");
  Serial.println("IP address: ");
  printToLCD("IP address: ");
  Serial.println(WiFi.localIP());
  char* char_array = new char[20]; 
  strcpy(char_array,WiFi.localIP().toString().c_str());
  printToLCD(char_array);
}

void callback(char* topic, byte* payload, unsigned int length) {
  // string to lcd
  String messageTemp;
  String numberMessageTemp;
  char statusReport[10];
  String relay1 = "MainGate/CMD/Relay1";
  String relay2 = "MainGate/CMD/Relay2";
  String relay3 = "MainGate/CMD/Relay3";
  String relay4 = "MainGate/CMD/Relay4";

  String statusReccurenceTopic = "MainGate/CMD/statusReccurence";
  String debounceSampleTimeTopic = "MainGate/CMD/debounceSampleTime";
  String relayProcessTimerTopic = "MainGate/CMD/relayProcessTimer";

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
	  messageTemp += (char)payload[i];
    if (isDigit((char)payload[i])) {
      // convert the incoming byte to a char and add it to the string:
      numberMessageTemp += (char)payload[i];
    }
  }
  Serial.println();

  if(relay1.compareTo(topic) == 0)
  {
    Serial.println("New number received on MQTT: " + messageTemp);
    if(messageTemp.compareTo("ON") == 0) { relayStatusArray[0] = 1; strcpy(statusReport, "ON");}
    if(messageTemp.compareTo("OFF") == 0) { relayStatusArray[0] = 0; strcpy(statusReport, "OFF");}
    client.publish("MainGate/STAT/Relay1", statusReport);
  }

  if(relay2.compareTo(topic) == 0)
  {
    Serial.println("New text received on MQTT: " + messageTemp);
    if(messageTemp.compareTo("ON") == 0) { 
      //relayStatusArray[1] = 1; strcpy(statusReport, "ON");}
      //special case open and close relay within 2 seconds or the coil will burn
      openGate();
      // will publish the statusReport in openGate no need to publish here
    }
    if(messageTemp.compareTo("OFF") == 0) { 
      relayStatusArray[1] = 0; 
      strcpy(statusReport, "OFF");
      client.publish("MainGate/STAT/Relay2", statusReport);
    }
  }

  if(relay3.compareTo(topic) == 0)
  {
    Serial.println("New text received on MQTT: " + messageTemp);
    if(messageTemp.compareTo("ON") == 0) { relayStatusArray[2] = 1; strcpy(statusReport, "ON");}
    if(messageTemp.compareTo("OFF") == 0) { relayStatusArray[2] = 0; strcpy(statusReport, "OFF");}
    client.publish("MainGate/STAT/Relay3", statusReport);
  }

  if(relay4.compareTo(topic) == 0)
  {
    Serial.println("New text received on MQTT: " + messageTemp);
    if(messageTemp.compareTo("ON") == 0) { relayStatusArray[3] = 1; strcpy(statusReport, "ON");}
    if(messageTemp.compareTo("OFF") == 0) { relayStatusArray[3] = 0; strcpy(statusReport, "OFF");}
    client.publish("MainGate/STAT/Relay4", statusReport);
  }

  if(statusReccurenceTopic.compareTo(topic) == 0) {
    char charArrayNumberMessageTemp[31];
    int newReccurence;
    Serial.println("New text received on MQTT: " + numberMessageTemp);
    newReccurence = numberMessageTemp.toInt();
    if(newReccurence <= 80000 && newReccurence > 1000)
    {
       statusReportReccurence = newReccurence;
       numberMessageTemp.toCharArray(charArrayNumberMessageTemp,30);
       client.publish("MainGate/STAT/statusReccurence", charArrayNumberMessageTemp);
       timer.cancel();
       timer.every(statusReportReccurence, mqttStatusReporting);
    }
    else
    {
      strcpy(statusReport, "Invalid");
      client.publish("MainGate/STAT/statusReccurence", statusReport);
    }
  }

  if(debounceSampleTimeTopic.compareTo(topic) == 0) {
    char charArrayNumberMessageTemp[31];
    int newReccurence;
    Serial.println("New text received on MQTT: " + numberMessageTemp);
    newReccurence = numberMessageTemp.toInt();
    if(newReccurence <= 80000 && newReccurence >= 1000)
    {
       debounceSampleTime = newReccurence;
       numberMessageTemp.toCharArray(charArrayNumberMessageTemp,30);
       client.publish("MainGate/STAT/debounceSampleTime", charArrayNumberMessageTemp);  
    }
    else
    {
      strcpy(statusReport, "Invalid");
      client.publish("MainGate/STAT/debounceSampleTime", statusReport);
    }
  }

  if(relayProcessTimerTopic.compareTo(topic) == 0) {
    char charArrayNumberMessageTemp[31];
    int newReccurence;
    Serial.println("New text received on MQTT: " + numberMessageTemp);
    newReccurence = numberMessageTemp.toInt();
    if(newReccurence <= 80000 && newReccurence >= 1000)
    {
       relayGateOpenProcessingTime = newReccurence;
       numberMessageTemp.toCharArray(charArrayNumberMessageTemp,30);
       client.publish("MainGate/STAT/relayProcessTimer", charArrayNumberMessageTemp);
    }
    else
    {
      strcpy(statusReport, "Invalid");
      client.publish("MainGate/STAT/debounceSampleTime", statusReport);
    }
  }

  //process the messages that are arriving
  processRelayStatus();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-MainGate";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
	  client.subscribe("MainGate/CMD/Relay1");
	  client.subscribe("MainGate/CMD/Relay2");
	  client.subscribe("MainGate/CMD/Relay3");
	  client.subscribe("MainGate/CMD/Relay4");

    client.subscribe("MainGate/CMD/statusReccurence");
    client.subscribe("MainGate/CMD/debounceSampleTime");
    client.subscribe("MainGate/CMD/relayProcessTimer");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup()
{
	// initialize the serial port for debug
	Serial.begin(115200);
	delay(1000);
	Serial.println("Intializing the ports...");
	
	// initialize the port for the relays
	initRelayPins();
  // intitialize the read out pins
  initInputPins();

	Serial.println("Ready to start");

	//setup the mqtt connection
	//pinMode(BUILTIN_LED, OUTPUT);   
	setup_wifi();
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
	printToLCD("Boot-up ...");

  // init status reporting on mqtt every reccurence setting default 30s
  timer.every(statusReportReccurence, mqttStatusReporting);
}

void loop()
{
	if (!client.connected()) {
    reconnect();
	}
  timer.tick();
  relayProcessTimer.tick();
  debounceTimer.tick();
	client.loop();
}