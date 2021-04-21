const int OUTPUT_PIEZO_PIN = 3;
const int INPUT_FSR_PIN = A0;
const int DELAY_MS = 20;
int freqs[8] = {262, 294, 330, 350, 392, 440, 494, 523};
int leds[8] = {4, 5, 6, 7, 8, 9, 10, 11};

void setup() {
  // put your setup code here, to run once:
  pinMode(OUTPUT_PIEZO_PIN, OUTPUT);
  pinMode(INPUT_FSR_PIN, INPUT);
  for (int i = 0; i < sizeof(leds); i++) {
    pinMode(i, OUTPUT);
  }
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(INPUT_FSR_PIN);
  int index = map(val, 0, 950, 1, 8);
  if (val > 0) {
    tone(OUTPUT_PIEZO_PIN, freqs[index - 1]);
    digitalWrite(leds[index - 1], HIGH);
    for (int i = 0; i < sizeof(leds); i++) {
      if (i != index - 1) {
        digitalWrite(leds[i], LOW);
      }
    }
  } else {
    noTone(OUTPUT_PIEZO_PIN);
    for (int i = 0; i < sizeof(leds); i++) {
       digitalWrite(leds[i], LOW);
    }
  }
  
  Serial.print(index);
  Serial.print(" ");
  Serial.println(val);
  
  delay(DELAY_MS);
}
