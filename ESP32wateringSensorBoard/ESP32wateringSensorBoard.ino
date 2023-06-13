#include <esp_now.h>
#include <WiFi.h>

#define SECONDFACTOR 1000000  
#define TIME_TO_SLEEP  10

const int SensorNumber = 2;
const int minValue = 878;
const int maxValue = 2667;

const int SensorPin = 32;
int readValue;
int mappedValue;

uint8_t responderAddress[] = {0x0C, 0xB8, 0x15, 0x77, 0x0E, 0xD4};

typedef struct struct_message {
  int sensorNumber;
  int mappedValue;
} struct_message;

struct_message data;

esp_now_peer_info_t peerInfo;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  esp_deep_sleep_start();
}

void onSendFail() {
  esp_deep_sleep_start();
}

void setup() {
  WiFi.mode(WIFI_STA);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * SECONDFACTOR);

  readValue = analogRead(SensorPin);
  mappedValue = map(readValue, minValue, maxValue, 100, 0);
  if (mappedValue > 100) {
    mappedValue = 100;
  }
  if (mappedValue < 0) {
    mappedValue = 0;
  }

  data.sensorNumber = SensorNumber;
  data.mappedValue = mappedValue;

  if (esp_now_init() != ESP_OK) {
    onSendFail();
    
    return;
  }

  esp_now_register_send_cb(onDataSent);

  memcpy(peerInfo.peer_addr, responderAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    onSendFail();    
    
    return ;
  }

  esp_err_t result = esp_now_send(responderAddress, (uint8_t *) &data, sizeof(data));
}

void loop() {}