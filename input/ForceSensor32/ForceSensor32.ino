const int OUTPUT_PIEZO_PIN = 3;
const int INPUT_FSR_PIN = A5;
const int DELAY_MS = 20;
const int GREEN_PIN = 21;
const int YELLOW_PIN = 17;
const int RED_PIN = 16;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(INPUT_FSR_PIN, INPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(INPUT_FSR_PIN);
  Serial.print("Val: ");
  Serial.println(val);
  int index = map(val, 0, 4095, 0, 3);
  Serial.print("index: ");
  Serial.println(index);
  
  
  if (index >= 1) {
    digitalWrite(GREEN_PIN, HIGH);
  }
  if (index >= 2) {
    digitalWrite(YELLOW_PIN, HIGH);
  }
  
  if (index >= 3) {
    Serial.print("red hi");
    digitalWrite(RED_PIN, HIGH);
  }

  if (index == 0) {
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(RED_PIN, LOW);
  }
  
  delay(DELAY_MS);
}
