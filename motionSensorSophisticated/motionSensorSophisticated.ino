int ledPin = 4;
int inputPin = 3;
int pirState = true;
int val = 0;
int minSecAfterPIRLow = 5000;

long unsigned int timeTurnedLow;
boolean takeLowTime;

int calibrationTime = 30;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT);

  takeLowTime = LOW;

  Serial.begin(9600);
  Serial.println("Waiting for sensor to warm up...");
  delay(calibrationTime * 1000);
}

void loop() {
  val = digitalRead(inputPin);

  if (val == HIGH) {
    digitalWrite(ledPin, HIGH);
    if (pirState == LOW) {
      Serial.println("Motion detected");
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH) {
      Serial.println("Motion ended");
      pirState = LOW;
      timeTurnedLow = millis();      
    }
  }

  if (!pirState && (millis() - timeTurnedLow) > minSecAfterPIRLow) {
    Serial.println("Extended time HIGH ended");
    digitalWrite(ledPin, LOW);
  }  
  delay(1000);
}
