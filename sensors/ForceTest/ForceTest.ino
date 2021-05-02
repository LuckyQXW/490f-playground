const int INPUT_FSR_PIN = A0;
const int DELAY_MS = 20;

void setup() {
  // put your setup code here, to run once:
  pinMode(INPUT_FSR_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(INPUT_FSR_PIN);
  Serial.println(val);
  
  delay(DELAY_MS);
}
