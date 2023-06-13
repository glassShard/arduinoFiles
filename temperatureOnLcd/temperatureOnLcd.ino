#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
#define relayPin 3
#define SMSPin1 A0
#define SMSPin2 A1
#define SMSPin3 A2
#define SMSPin4 A3

int buttonPin = 4;
int buttonState = 0;
unsigned long milliSecs;
int delayTime = 3000;
int heating = 0;
int initValue = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  milliSecs = millis();
  pinMode(buttonPin, INPUT); 
  pinMode(relayPin, OUTPUT);  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("Temperature");
  lcd.setCursor(5,1);
  lcd.print("Sensor");
  delay(3000);
  //lcd.noBacklight();
  lcd.clear();
  dht.begin();
}

void loop() {
  delay(1000);
  buttonState = digitalRead(buttonPin);
  Serial.begin(9600);
  /*if (buttonState == HIGH) {
    lcd.backlight();  
  } else {
    lcd.noBacklight();
  }*/

  unsigned long millisNow = millis();

  if (millisNow - milliSecs > delayTime) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if(t < 22) {
      digitalWrite(relayPin, HIGH);
      heating = 1;
    } 

    if (t > 26) {
      digitalWrite(relayPin, LOW);
      heating = 0;
    } 

    if (t >= 25 && t <= 30) {
      if (initValue == 0) {
        heating = 1;
        initValue = 1;        
        digitalWrite(relayPin, HIGH);
      }
    } 
    
    if (isnan(h) || isnan(t) || isnan(f)) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Failed to read");
      return;
    }

    float hif = dht.computeHeatIndex(f, h);
    //float hic = dht.computeHeatIndex(t, h, false);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(t);
    lcd.print("C ");
    lcd.print(h); 
    lcd.print("%"); 
 

    milliSecs = millisNow;      
  } 
  
  lcd.setCursor(0,1);
  lcd.print("1");
  lcd.print(map(analogRead(SMSPin1), 201, 470, 99, 0));
  lcd.print(" 2");
  lcd.print(map(analogRead(SMSPin2), 174, 460, 99, 0));
  lcd.print(" 3");
  lcd.print(map(analogRead(SMSPin3), 272, 490, 99, 0)); 
  lcd.print(" 4");
  lcd.print(map(analogRead(SMSPin4), 192, 463, 99, 0));
}
