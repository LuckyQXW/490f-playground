const int VIBRO_PIN = 3;
const int BTN_PIN = 5;
const int MAX_ANALOG_INPUT = 1023;

int _interval = 1;
const int MAX_INTERVAL_LENGTH = 15;

void setup() {
  pinMode(VIBRO_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
}

void loop() {
  // breathing pattern- from frequent vibration to less frequent, increasing interval
  int btnVal = digitalRead(BTN_PIN);
  if (btnVal == LOW) {
    analogWrite(VIBRO_PIN, MAX_ANALOG_INPUT * 0.95);
    delay(50);
    _interval += 1;
    analogWrite(VIBRO_PIN, 0);
    delay(_interval/2 * 30);
    if (_interval > MAX_INTERVAL_LENGTH) {
      _interval = 1;
      delay(5000);
    }
  }
}
