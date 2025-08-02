#include <WiFi.h>
#include <esp_wifi.h>
#include <PubSubClient.h>

#define SECONDFACTOR 1000000  
#define TIME_TO_SLEEP  600

const int sensorNumber = 2;
const char* clientName = "Sensor3";
const int minValue = 981;
const int maxValue = 2735;
const char* ssid = "DIGI_0b2d70";
const char* password = "XXXXXXXX";
const char* mqtt_server = "192.168.1.10";

WiFiClient espClient;
PubSubClient client(espClient);

const int SensorPin = 32;
int readValue;
int mappedValue;

void setup_wifi() {
  delay(50);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int c=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    c = c + 1;
    if (c > 10){
      ESP.restart();
    }
  }
}

void connect_mqttServer() {
  while (!client.connected()) {
    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
    }
    client.connect(clientName);
    if (!client.connected()) {
      delay(1000);
    }
  }  
}

void setup() {
  Serial.begin(115200);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * SECONDFACTOR);

  readValue = analogRead(SensorPin);
  mappedValue = map(readValue, minValue, maxValue, 100, 0);
  if (mappedValue > 100) {
    mappedValue = 100;
  }
  if (mappedValue < 0) {
    mappedValue = 0;
  }

  char msgOut[20];
  sprintf(msgOut, "%d - %d", sensorNumber, mappedValue);

  Serial.print("readValue = ");
  Serial.println(readValue);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  connect_mqttServer();

  client.publish("water", msgOut);
  client.disconnect();

  esp_deep_sleep_start();
}

void loop() {}