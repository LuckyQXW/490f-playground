const int LED_OUTPUT_1 = 3;
const int LED_OUTPUT_2 = 4;
const int DELAY_MS = 1000;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_OUTPUT_1, OUTPUT);
  pinMode(LED_OUTPUT_2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_OUTPUT_1, HIGH);
  digitalWrite(LED_OUTPUT_2, HIGH);

  delay(DELAY_MS);

  digitalWrite(LED_OUTPUT_1, LOW);
  digitalWrite(LED_OUTPUT_2, LOW);

  delay(DELAY_MS);
  
}
