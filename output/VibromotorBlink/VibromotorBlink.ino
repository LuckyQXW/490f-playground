/*
  Vibratormotor with blink
*/

const int VIBRO_PIN = 3;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(VIBRO_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(VIBRO_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(VIBRO_PIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
