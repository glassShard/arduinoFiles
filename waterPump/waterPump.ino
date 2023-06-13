#define relayPin 5
#define sensorPin A0

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(sensorPin, INPUT);

  Serial.begin(9600);
}

void loop() {
  delay(50);
  int sensorRead = analogRead(sensorPin);
  // put your main code here, to run repeatedly:
  Serial.println(sensorRead);
  if (sensorRead < 700) {
    digitalWrite(relayPin, HIGH);    
  } else {
    digitalWrite(relayPin, LOW);
  }
}