const int LED_OUTPUT_PIN = 3;
const int MAX_ANALOG_OUT = 255; // the max analog output on the Uno is 255
const int DELAY_MS = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_OUTPUT_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i <= MAX_ANALOG_OUT; i++) {
    analogWrite(LED_OUTPUT_PIN, i);
    delay(DELAY_MS);
  }
  
  for (int i = MAX_ANALOG_OUT; i >= 0; i--) {
    analogWrite(LED_OUTPUT_PIN, i);
    delay(DELAY_MS);
  }
}
