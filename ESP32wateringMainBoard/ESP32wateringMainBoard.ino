#include <esp_now.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <iostream>
#include <string>

#define TIME_TO_WATER 300000
#define DELAY 60000
#define COUNT 10

WiFiClientSecure client;

const char* ssid = "DIGI_0b2d70";
const char* password = "aa21f4d2";
const char* server = "turazzunk.hu";
const char* url = "/watering/uploadSensorData.php";
const char* test_root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFuzCCA6OgAwIBAgIIVwoRl0LE48wwDQYJKoZIhvcNAQELBQAwazELMAkGA1UE\n" \
"BhMCSVQxDjAMBgNVBAcMBU1pbGFuMSMwIQYDVQQKDBpBY3RhbGlzIFMucC5BLi8w\n" \
"MzM1ODUyMDk2NzEnMCUGA1UEAwweQWN0YWxpcyBBdXRoZW50aWNhdGlvbiBSb290\n" \
"IENBMB4XDTExMDkyMjExMjIwMloXDTMwMDkyMjExMjIwMlowazELMAkGA1UEBhMC\n" \
"SVQxDjAMBgNVBAcMBU1pbGFuMSMwIQYDVQQKDBpBY3RhbGlzIFMucC5BLi8wMzM1\n" \
"ODUyMDk2NzEnMCUGA1UEAwweQWN0YWxpcyBBdXRoZW50aWNhdGlvbiBSb290IENB\n" \
"MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAp8bEpSmkLO/lGMWwUKNv\n" \
"UTufClrJwkg4CsIcoBh/kbWHuUA/3R1oHwiD1S0eiKD4j1aPbZkCkpAW1V8IbInX\n" \
"4ay8IMKx4INRimlNAJZaby/ARH6jDuSRzVju3PvHHkVH3Se5CAGfpiEd9UEtL0z9\n" \
"KK3giq0itFZljoZUj5NDKd45RnijMCO6zfB9E1fAXdKDa0hMxKufgFpbOr3JpyI/\n" \
"gCczWw63igxdBzcIy2zSekciRDXFzMwujt0q7bd9Zg1fYVEiVRvjRuPjPdA1Yprb\n" \
"rxTIW6HMiRvhMCb8oJsfgadHHwTrozmSBp+Z07/T6k9QnBn+locePGX2oxgkg4YQ\n" \
"51Q+qDp2JE+BIcXjDwL4k5RHILv+1A7TaLndxHqEguNTVHnd25zS8gebLra8Pu2F\n" \
"be8lEfKXGkJh90qX6IuxEAf6ZYGyojnP9zz/GPvG8VqLWeICrHuS0E4UT1lF9gxe\n" \
"KF+w6D9Fz8+vm2/7hNN3WpVvrJSEnu68wEqPSpP4RCHiMUVhUE4Q2OM1fEwZtN4F\n" \
"v6MGn8i1zeQf1xcGDXqVdFUNaBr8EBtiZJ1t4JWgw5QHVw0U5r0F+7if5t+L4sbn\n" \
"fpb2U8WANFAoWPASUHEXMLrmeGO89LKtmyuy/uE5jF66CyCU3nuDuP/jVo23Eek7\n" \
"jPKxwV2dpAtMK9myGPW1n0sCAwEAAaNjMGEwHQYDVR0OBBYEFFLYiDrIn3hm7Ynz\n" \
"ezhwlMkCAjbQMA8GA1UdEwEB/wQFMAMBAf8wHwYDVR0jBBgwFoAUUtiIOsifeGbt\n" \
"ifN7OHCUyQICNtAwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3DQEBCwUAA4ICAQAL\n" \
"e3KHwGCmSUyIWOYdiPcUZEim2FgKDk8TNd81HdTtBjHIgT5q1d07GjLukD0R0i70\n" \
"jsNjLiNmsGe+b7bAEzlgqqI0JZN1Ut6nna0Oh4lScWoWPBkdg/iaKWW+9D+a2fDz\n" \
"WochcYBNy+A4mz+7+uAwTc+G02UQGRjRlwKxK3JCaKygvU5a2hi/a5iB0P2avl4V\n" \
"SM0RFbnAKVy06Ij3Pjaut2L9HmLecHgQHEhb2rykOLpn7VU+Xlff1ANATIGk0k9j\n" \
"pwlCCRT8AKnCgHNPLsBA2RF7SOp6AsDT6ygBJlh0wcBzIm2Tlf05fbsq4/aC4yyX\n" \
"X04fkZT6/iyj2HYauE2yOE+b+h1IYHkm4vP9qdCa6HCPSXrW5b0KDtst842/6+Ok\n" \
"fcvHlXHo2qN8xcL4dJIEG4aspCJTQLas/kx2z/uUMsA1n3Y/buWQbqCmJqK4LL7R\n" \
"K4X9p2jIugErsWx0Hbhzlefut8cl8ABMALJ+tguLHPPAUJ4lueAI3jZm/zel0btU\n" \
"ZCzJJ7VLkn5l/9Mt4blOvH+kQSGQQXemOR/qnuOf0GZvBeyqdn6/axag67XH/JJU\n" \
"LysRJyU3eExRarDzzFhdFPFqSBX/wge2sY0PjlxQRrM9vwGYT7JZVEc+NHt4bVaT\n" \
"LnPqZih4zR0Uv6CPLy64Lo7yFIrM6bV8+2ydDKXhlg==\n" \
"-----END CERTIFICATE-----\n";

uint8_t relayPins[4] = {33, 32, 26, 25};
int counter = 0;

typedef struct struct_message {
  int sensorNumber;
  int mappedValue;
} struct_message;

struct_message sensorData;

typedef struct circle_data {
    int number;
    bool relayMode;
    uint relayMinutes;   
    int sensorValue;
    unsigned long timeStarted;
} circle_data;

circle_data circles[] = {
    { .number = 1, .relayMode = false, .relayMinutes = 0, .sensorValue = 100, .timeStarted = 0 },
    { .number = 2, .relayMode = false, .relayMinutes = 0, .sensorValue = 100, .timeStarted = 0 },
    { .number = 3, .relayMode = false, .relayMinutes = 0, .sensorValue = 100, .timeStarted = 0 },
    { .number = 4, .relayMode = false, .relayMinutes = 0, .sensorValue = 100, .timeStarted = 0 }
};

void checkRelays() {
  unsigned int i = 0;
  
  for (i = 0; i<4; i++) {
    Serial.print("Checking relay ");
    Serial.print(i + 1);
    Serial.print("... relay state is ");
    Serial.print(circles[i].relayMode);

    if (circles[i].relayMode == true) {
      circles[i].relayMinutes += 1;
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

void resetRelayMinutes() {
  unsigned int i = 0;

  for (i = 0; i<4; i++) {
    circles[i].relayMinutes = 0;
  }
}

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&sensorData, incomingData, sizeof(sensorData));
  Serial.print("Sensor number: ");
  Serial.println(sensorData.sensorNumber);
  Serial.print("Moisture value: ");
  Serial.println(sensorData.mappedValue);
  Serial.println();

  unsigned int index = sensorData.sensorNumber - 1;
  circles[index].sensorValue = sensorData.mappedValue;

  if (sensorData.mappedValue < 20) {  
    if (circles[index].relayMode == false) {
      circles[index].relayMode = true;
      digitalWrite(relayPins[index], HIGH);
      circles[index].timeStarted = millis();
    }   
  }  
}

void sendDataToServer() {
  WiFiClientSecure client;

  client.setCACert(test_root_ca);

  String postData = "sensor={\"soil1\": " + String(circles[0].sensorValue) 
    + ", \"soil2\": " + String(circles[1].sensorValue) 
    + ", \"soil3\": " + String(circles[2].sensorValue)
    + ", \"soil4\": " + String(circles[3].sensorValue) 
    + ", \"relay1\": " + String(circles[0].relayMinutes)
    + ", \"relay2\": " + String(circles[1].relayMinutes)
    + ", \"relay3\": " + String(circles[2].relayMinutes)
    + ", \"relay4\": " + String(circles[3].relayMinutes) + "}";

  Serial.println(postData);

  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    client.println(String("POST https://") + server + url + " HTTP/1.0");
    client.println(String("Host: ") + server);
    client.println("Content-type: application/x-www-form-urlencoded");   
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);

    client.stop();
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);

  pinMode(relayPins[0], OUTPUT);
  pinMode(relayPins[1], OUTPUT);
  pinMode(relayPins[2], OUTPUT);
  pinMode(relayPins[3], OUTPUT);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP_NOW");

    return;
  }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

  esp_now_register_recv_cb(onDataRecv);  

  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
}


void loop() {
  checkRelays();
  Serial.println(counter);
  if (counter == COUNT - 1) {
    sendDataToServer();
    resetRelayMinutes();
  }
  counter += 1;
  if (counter == COUNT) {
    counter = 0;
  }
  delay(DELAY);
}