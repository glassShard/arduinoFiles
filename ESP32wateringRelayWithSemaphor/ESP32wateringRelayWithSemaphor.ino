#include <WiFi.h>
#include <esp_wifi.h>
#include <PubSubClient.h>
#include <string>
#include <math.h>
#include <esp_task_wdt.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define DELAY 60000
#define LIMIT 65
#define WDT_TIMEOUT 20
#define WD_DELAY 10000
#define SIDES 120000
#define MIDDLE 120000
#define GRASS 600000
#define OUTSIDE 300000

const char* ssid = "DIGI_0b2d70";
const char* password = "XXXXXXXX";
const char* mqtt_server = "192.168.1.10";

WiFiClient espClient;
PubSubClient client(espClient);

uint8_t relayPins[4] = {33, 32, 26, 25};
int counter = 0;
int wd_counter = 0;

SemaphoreHandle_t xSemaphore;

typedef struct circle_data {
  int number;
  bool relayMode;
  unsigned long timeStarted;
  int water;
} circle_data;

circle_data circles[] = {
  { .number = 1, .relayMode = false, .timeStarted = 0, .water = SIDES },
  { .number = 2, .relayMode = false, .timeStarted = 0, .water = MIDDLE },
  { .number = 3, .relayMode = false, .timeStarted = 0, .water = GRASS },
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

  Serial.print("relay ");
  Serial.print(index);
  Serial.print(" state: ");
  Serial.println(relayPins[index]);

  if (value == "ON") {
    if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE) {
      if (digitalRead(relayPins[index]) == HIGH) {
        digitalWrite(relayPins[index], LOW);
        circles[index].relayMode = true;
        Serial.println("if !cirles[index].relayMOde");        
        circles[index].timeStarted = millis();
      }
      xSemaphoreGive(xSemaphore);
    }
  }

  if (value == "OFF") {
    if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE) {
      if (digitalRead(relayPins[index]) == LOW) {        
        digitalWrite(relayPins[index], HIGH);
        circles[index].relayMode = false;
        int relayMillis = millis() - circles[index].timeStarted;
        circles[index].timeStarted = 0;
        sendRelayWater(index + 1, relayMillis);
      }
      xSemaphoreGive(xSemaphore);
    }
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

    if (digitalRead(relayPins[i]) == LOW) {
      int relayMillis = circles[i].timeStarted + circles[i].water;
      if (relayMillis < millis()) {
        if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE) {
          digitalWrite(relayPins[i], HIGH);
          Serial.println(" relay is switched off.");
          circles[i].relayMode = false;
          circles[i].timeStarted = 0;
          sendRelayWater(i + 1, millis() - circles[i].timeStarted);
          xSemaphoreGive(xSemaphore);
        }
                       
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
    if (c > 20){
      ESP.restart();
    }
  }
}

void connect_mqttServer() {
  while (!client.connected()) {
    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
    }
    client.connect("Main");
    if (!client.connected()) {
      delay(1000);
    }
  }
          
  client.setCallback(callback);
  client.subscribe("relay", 1);
  
  Serial.println("MQTT Connected");  
}

void setup() {
  Serial.println("Configuring WDT...");
  
  /*esp_task_wdt_config_t config = {
    .timeout_ms = WDT_TIMEOUT * 1000,
    .trigger_panic = true,
  };*/
  enableLoopWDT();

  //esp_task_wdt_init(&config);
  esp_task_wdt_add(NULL);
  
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(115200);

  pinMode(relayPins[0], OUTPUT);
  pinMode(relayPins[1], OUTPUT);
  pinMode(relayPins[2], OUTPUT);
  pinMode(relayPins[3], OUTPUT);

  for (int i = 0; i < 4; i++) {
    digitalWrite(relayPins[i], HIGH);
  }

  setup_wifi();
  client.setServer(mqtt_server,1883);
  connect_mqttServer();

  xSemaphore = xSemaphoreCreateMutex();
}

void loop() {
  if (!client.connected()) {
    connect_mqttServer();
  }
  
  client.loop();
  int timePassed = millis() - counter;
  int timePassedWD = millis() - wd_counter;
  
  /*if (timePassedWD >= WD_DELAY) {
    Serial.println(timePassedWD),
    Serial.println("WatchDog reset");
    wd_counter = millis();
    esp_task_wdt_reset();
  }*/
  
  if (timePassed >= DELAY) {
      Serial.println(timePassed);
      counter = millis();
      checkRelays();
  }
}

