int ledPin = 4;
int inputPin = 3;
int pirState = LOW;
int val = 0;

int calibrationTime = 30;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT);

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
    digitalWrite(ledPin, LOW);
    if (pirState == HIGH) {
      Serial.println("Motion ended");
      pirState = LOW;
    }
  }
  delay(1000);
}
