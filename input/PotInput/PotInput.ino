const int LED_OUTPUT_PIN = 3;
const float RATIO = 255.0/2013;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  int potVal = analogRead(A0);
  analogWrite(LED_OUTPUT_PIN, potVal * RATIO);
  Serial.println(potVal);
}
