const int INPUT_BUTTON_PIN1 = 12;
const int INPUT_BUTTON_PIN2 = 11;
const int INPUT_BUTTON_PIN3 = 10;

const int OUTPUT_LED_PIN1 = 4;
const int OUTPUT_LED_PIN2 = 3;
const int OUTPUT_LED_PIN3 = 2;
void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_BUTTON_PIN1, INPUT);
  pinMode(INPUT_BUTTON_PIN2, INPUT);
  pinMode(INPUT_BUTTON_PIN3, INPUT_PULLUP);
  pinMode(OUTPUT_LED_PIN1, OUTPUT);
  pinMode(OUTPUT_LED_PIN2, OUTPUT);
  pinMode(OUTPUT_LED_PIN3, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonVal1 = digitalRead(INPUT_BUTTON_PIN1);
  int buttonVal2 = digitalRead(INPUT_BUTTON_PIN2);
  int buttonVal3 = digitalRead(INPUT_BUTTON_PIN3);
  Serial.print(buttonVal1);
  Serial.print(",");
  Serial.print(buttonVal2);
  Serial.print(",");
  Serial.println(buttonVal3);
  digitalWrite(OUTPUT_LED_PIN1, buttonVal1);
  digitalWrite(OUTPUT_LED_PIN2, buttonVal2);
  digitalWrite(OUTPUT_LED_PIN3, buttonVal3);
  delay(5);
}
