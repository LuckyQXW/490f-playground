const int DELAY_MS = 5;
const int OUTPUT_PIN1 = 5;
const int OUTPUT_PIN2 = 6;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(OUTPUT_PIN1, OUTPUT);
  pinMode(OUTPUT_PIN2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    String rcvdIdx = Serial.readStringUntil(',');
    int ledIndex = rcvdIdx.toInt();
    String rcvdVal = Serial.readStringUntil('\n');
    int ledVal = rcvdVal.toInt();
    ledVal = constrain(ledVal, 0, 255);
    if (ledIndex == 0) {
      analogWrite(OUTPUT_PIN1, ledVal);
    } else {
      analogWrite(OUTPUT_PIN2, ledVal);
    }
    Serial.print("Arduino received: '");
    Serial.print(rcvdVal);
    Serial.println("'");
  }
}
