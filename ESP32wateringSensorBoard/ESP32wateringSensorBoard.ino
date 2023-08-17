#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define SECONDFACTOR 1000000  
#define TIME_TO_SLEEP  600

const int SensorNumber = 4;
const int minValue = 873;
const int maxValue = 2559;
const char* ssid = "DIGI_0b2d70";

const int SensorPin = 32;
int readValue;
int mappedValue;

//uint8_t responderAddress[] = {0x00, 0xBB, 0x11, 0x77, 0x00, 0xDD};
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
  Serial.println("Sending data failed!");
  esp_deep_sleep_start();
}

int32_t obtain_wifi(const char *ssid) {
  Serial.println("Function obtain_wifi called");
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}

void setup() {
  WiFi.mode(WIFI_STA);

  Serial.begin(115200);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * SECONDFACTOR);

  int32_t channel = obtain_wifi(ssid);
  Serial.print("Channel: ");
  Serial.println(channel);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

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
  Serial.print("readValue = ");
  Serial.println(readValue);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Esp now init failed!");
    onSendFail();
    
    return;
  }

  esp_now_register_send_cb(onDataSent);

  memcpy(peerInfo.peer_addr, responderAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Esp add peer failed!");
    onSendFail();    
    
    return ;
  } else {
    Serial.println("ESP add peer succeed!");     
  }

  esp_err_t result = esp_now_send(responderAddress, (uint8_t *) &data, sizeof(data));
}

void loop() {}