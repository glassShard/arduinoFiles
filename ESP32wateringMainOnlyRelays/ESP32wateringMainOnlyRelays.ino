#include <esp_now.h>
#include <WiFi.h>

#define TIME_TO_WATER 30000
#define DELAY 20000

uint8_t relayPins[4] = {33, 32, 26, 25};

typedef struct struct_message {
  int sensorNumber;
  int mappedValue;
} struct_message;

struct_message sensorData;

typedef struct circle_data {
    int number;
    bool relayMode;
    int sensorValue;
    unsigned long timeStarted;
} circle_data;

circle_data circles[] = {
    { .number = 1, .relayMode = false, .sensorValue = 100, .timeStarted = 0 },
    { .number = 2, .relayMode = false, .sensorValue = 100, .timeStarted = 0 },
    { .number = 3, .relayMode = false, .sensorValue = 100, .timeStarted = 0 },
    { .number = 4, .relayMode = false, .sensorValue = 100, .timeStarted = 0 }
};

void checkRelays() {
  unsigned int i = 0;
  
  for (i = 0; i<4; i++) {
    Serial.print("Checking relay ");
    Serial.print(i + 1);
    Serial.print("... relay state is ");
    Serial.print(circles[i].relayMode);

    if (circles[i].relayMode == true) {
      if (circles[i].timeStarted + (TIME_TO_WATER) < millis()) {
        digitalWrite(relayPins[i], LOW);
        circles[i].relayMode = false;

        Serial.println(" relay is swithed off.");        
      } else {
        Serial.println(" relay is ok.");
      } 
    } else {
      Serial.println(" relay is ok.");
    }
  }
}

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&sensorData, incomingData, sizeof(sensorData));
  Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("Sensor number: ");
  Serial.println(sensorData.sensorNumber);
  Serial.print("Moisture value: ");
  Serial.println(sensorData.mappedValue);
  Serial.println();

  if (sensorData.mappedValue < 20) {
    unsigned int index = sensorData.sensorNumber - 1;
    if (circles[index].relayMode == false) {
      circles[index].relayMode = true;
      digitalWrite(relayPins[index], HIGH);
      circles[index].timeStarted = millis();
    }   
  }  
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  pinMode(relayPins[0], OUTPUT);
  pinMode(relayPins[1], OUTPUT);
  pinMode(relayPins[2], OUTPUT);
  pinMode(relayPins[3], OUTPUT);  

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP_NOW");

    return;
  }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

  esp_now_register_recv_cb(onDataRecv);
}


void loop() {
  checkRelays();
  delay(DELAY);
}

41	7	4	MRDG - DB3 - TEST	2024-02-06 17:19:21	2024-02-06 17:19:21	idg-test
42	24	4	MRDG - DB3 - TEST	2024-02-06 17:19:21	2024-02-06 17:19:21	isg-test-rack
43	25	4	MRDG - DB3 - TEST	2024-02-06 17:19:21	2024-02-06 17:19:21	isg-test-gp
44	26	4	MRDG - DB3 - TEST	2024-02-06 17:19:21	2024-02-06 17:19:21	isg-test-msft
85	30	4	MRDG - DB3 - TEST	2024-02-19 13:07:36	2024-02-19 13:07:36	isg-test-dwc

