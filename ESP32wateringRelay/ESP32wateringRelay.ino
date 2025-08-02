#include <WiFi.h>
#include <esp_wifi.h>
#include <PubSubClient.h>
#include <string>
#include <math.h>
#include <esp_task_wdt.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define DELAY 60000
#define LIMIT 65
#define ANCSA 60000
#define MIDDLE 60000
#define OCSI 60000
#define OUTSIDE 180000

const char* ssid = "DIGI_0b2d70";
const char* password = "XXXXXXXX";
const char* mqtt_server = "192.168.1.10";

WiFiClient espClient;
PubSubClient client(espClient);

uint8_t relayPins[4] = {33, 32, 26, 25};
int counter = 0;

typedef struct circle_data {
    int number;
    bool relayMode;
    unsigned long timeStarted;
    int water;
} circle_data;

circle_data circles[] = {
    { .number = 1, .relayMode = false, .timeStarted = 0, .water = OCSI },
    { .number = 2, .relayMode = false, .timeStarted = 0, .water = MIDDLE },
    { .number = 3, .relayMode = false, .timeStarted = 0, .water = ANCSA },
    { .number = 4, .relayMode = false, .timeStarted = 0, .water = OUTSIDE }
};

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  char relayNumber; 
  String value = "";
  
  for (int i = 0; i < length; i++) {
    if (i == 0) {
      relayNumber = (char)payload[i];
    }
    if (i > 3) {
      value += (char)payload[i];
    }
    message += (char)payload[i];
  }

  Serial.println(message);
  Serial.print("Relay number is: ");
  Serial.println(relayNumber);
  Serial.print("Value is: ");
  Serial.println(value);

  int relay = relayNumber - '0';
  unsigned int index = relay - 1;

  if (value == "ON" && circles[index].relayMode == false) {
    circles[index].relayMode = true;
    digitalWrite(relayPins[index], HIGH);
    circles[index].timeStarted = millis();
  }

  if (value == "OFF" && circles[index].relayMode == true) {
    circles[index].relayMode = false;
    digitalWrite(relayPins[index], LOW);
    int relayMillis = millis() - circles[index].timeStarted;
    circles[index].timeStarted = 0;
    sendRelayWater(index + 1, relayMillis);
  } 
}

void sendRelayWater(int relay, int time) {
  Serial.print("SendRelayWater called, sent value is: ");
  char msgOut[10];
  int value = round(time / 1000);
  Serial.println(value);
  sprintf(msgOut, "%d - %d", relay, value);
  Serial.println(msgOut);
  client.publish("relayWater", msgOut);
}

void checkRelays() {
  unsigned int i = 0;
  
  for (i = 0; i<4; i++) {
    Serial.print("Checking relay ");
    Serial.print(i + 1);
    Serial.print("... relay state is ");
    Serial.print(circles[i].relayMode);

    if (circles[i].relayMode == true) {
      int relayMillis = circles[i].timeStarted + circles[i].water;
      if (relayMillis < millis()) {
        digitalWrite(relayPins[i], LOW);
        circles[i].relayMode = false;
        circles[i].timeStarted = 0;

        sendRelayWater(i + 1, relayMillis);
        Serial.println(" relay is swithed off.");        
      } else {
        Serial.println(" relay is ok.");
      } 
    } else {
      Serial.println(" relay is ok.");
    }
  }
}

void setup_wifi() {
  delay(50);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int c = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    c = c + 1;
    if(c > 20){
        ESP.restart();
    }
  }
}

void connect_mqttServer() {
  while (!client.connected()) {
    if(WiFi.status() != WL_CONNECTED){
      setup_wifi();
    }
    client.connect("Main");
    if (!client.connected()) {
      delay(2000);
    }
    Serial.println("MQTT Connected");
  }  
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  //esp_task_wdt_init(10, true);

  pinMode(relayPins[0], OUTPUT);
  pinMode(relayPins[1], OUTPUT);
  pinMode(relayPins[2], OUTPUT);
  pinMode(relayPins[3], OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server,1883);
  connect_mqttServer();
  
  client.setCallback(callback);
  client.subscribe("relay");
}

void loop() {
  connect_mqttServer();
  client.loop();
  int timePassed = millis() - counter;
  
  if (timePassed >= DELAY) {
    Serial.println(timePassed);
    counter = millis();
    checkRelays();
  }

  //esp_task_wdt_reset();
}

