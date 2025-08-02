#include <DS3231.h>
#include <Wire.h>
#include <time.h>
#include "DHT.h"
#include <LiquidCrystal_I2C.h>

DS3231 rtc;
uint8_t LIGHTPIN = A0;
uint8_t DHTPIN = 2;
uint8_t RELAYLIGHTPIN = 7;
uint8_t RELAYHEATPIN = 6;
uint8_t RELAYPUMPPIN = 8;
uint8_t RELAYTOWERLIGHTPIN = 9;
uint8_t relayLightState = LOW;
uint8_t relayHeatState = LOW;
uint8_t relayTowerLightState = LOW;
uint8_t relayPumpState = LOW;
unsigned long switchedOn = 1200000;
unsigned long switchedOff = 2400000;

unsigned long previousPumpSwitch = 0;

#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

String setLeadingZero(int value) {
  if (value < 10) {
    return 0 + String(value);
  }
  return String(value);
}

void setRelay(uint8_t relayPin, uint8_t value) {
  Serial.println("SetRelay called");
  Serial.println(relayPin);
  
  if (relayPin == RELAYLIGHTPIN) {
    relayLightState = value;
  }

  if (relayPin == RELAYHEATPIN) {
    relayHeatState = value;
  }

  if (relayPin == RELAYTOWERLIGHTPIN) {
    relayTowerLightState = value;
  }

  if (relayPin == RELAYPUMPPIN) {
    relayPumpState = value;
  }

  digitalWrite(relayPin, value);
}

void printOnLcd(float t, float h) {
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(t);
    lcd.print("C ");
    lcd.print(h); 
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Heat: ");
    if (relayHeatState == HIGH) {
      lcd.print("+ ");
    } else {
      lcd.print("- ");
    }
    lcd.print("Light: ");
    if (relayLightState == HIGH) {
      lcd.print("+");
    } else {
      lcd.print("-");
    }
}

void setup() {
	Serial.begin(9600);
	Wire.begin();

  pinMode(RELAYLIGHTPIN, OUTPUT);
  pinMode(RELAYHEATPIN, OUTPUT);
  pinMode(RELAYTOWERLIGHTPIN, OUTPUT);
  pinMode(RELAYPUMPPIN, OUTPUT);
  pinMode(LIGHTPIN, INPUT);
  pinMode(DHTPIN, INPUT);

  dht.begin();

  lcd.init();
  lcd.backlight();

  digitalWrite(RELAYHEATPIN, LOW);
  digitalWrite(RELAYLIGHTPIN, LOW);
  digitalWrite(RELAYTOWERLIGHTPIN, LOW);
  digitalWrite(RELAYPUMPPIN, LOW);

  const DateTime dnow = RTClib::now();
  Serial.print(dnow.year());
  Serial.print("-");
  Serial.print(setLeadingZero(dnow.month()));
  Serial.print("-");
  Serial.print(setLeadingZero(dnow.day()));
  Serial.print(" ");
  Serial.print(setLeadingZero(dnow.hour()));
  Serial.print(":");
  Serial.print(setLeadingZero(dnow.minute()));
  Serial.print(":");
  Serial.println(setLeadingZero(dnow.second()));
}

void loop() {
  
  const int light = analogRead(LIGHTPIN);
  Serial.println(light);
  const DateTime d = RTClib::now();
  
  if (relayLightState == HIGH) {
    if (d.hour() <= 4 || d.hour() >= 21 || light > 810) {
      setRelay(RELAYLIGHTPIN, LOW);
      setRelay(RELAYTOWERLIGHTPIN, LOW);
    } 
  }

  if (relayLightState == LOW) {
    if (d.hour() > 4 && d.hour() < 20 && light < 750) {
      setRelay(RELAYLIGHTPIN, HIGH);
      setRelay(RELAYTOWERLIGHTPIN, HIGH);
    } 
  }

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  Serial.println(t);
  Serial.println(h);

  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Failed to read");
    lcd.setCursor(0,1);
    lcd.print("from DHT sensor!");
  } else {
    printOnLcd(t, h);

    if (relayHeatState == LOW && t < 20) {
      Serial.println("set relay to high");
      setRelay(RELAYHEATPIN, HIGH);
    }

    if (relayHeatState == HIGH && t > 25) {
      Serial.println("set relay to low");
      setRelay(RELAYHEATPIN, LOW);
    }
  }

  if (relayPumpState == HIGH && millis() - previousPumpSwitch > switchedOn) {
    setRelay(RELAYPUMPPIN, LOW);
    previousPumpSwitch = millis(); 
  }
  if (relayPumpState == LOW && millis() - previousPumpSwitch > switchedOff) {
    setRelay(RELAYPUMPPIN, HIGH);
    previousPumpSwitch = millis(); 
  }

  delay(10000);
}
