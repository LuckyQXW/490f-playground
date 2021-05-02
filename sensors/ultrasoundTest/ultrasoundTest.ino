const int TRIG = 10;
const int ECHO = 11;

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  float distance = readDistanceSensor();
  Serial.print("Distance sensor read "); 
  Serial.print(distance);
  Serial.println(" cm.");
  delay(100);
}

float readDistanceSensor() {
   digitalWrite(TRIG, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG, LOW);
   float duration = pulseIn(ECHO, HIGH);  // in microseconds
   Serial.println(duration);
   float distance = duration * 0.0343 / 2.0;  // in centimeters
   return distance;
}
