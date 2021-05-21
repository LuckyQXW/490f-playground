const int POT_PIN = A0;
const int MAX_INPUT_VAL = 1023;
int _lastAnalogVal = -1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(POT_PIN, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(POT_PIN);
  float normalizedVal = val * 1.0 / MAX_INPUT_VAL;
  if (_lastAnalogVal != val) {
      Serial.println(normalizedVal, 4);
      _lastAnalogVal = val;
  }
  delay(5);
}
