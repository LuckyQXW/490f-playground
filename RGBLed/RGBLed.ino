const int RED = 3;
const int GREEN = 4;
const int BLUE = 5;
const boolean IS_COMMON_ANODE = false;

const int DELAY_MS = 1000;

void setup() {
  // put your setup code here, to run once:
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  Serial.begin(9600); 
}

void loop() {
    // red
  Serial.print("Red: ");
  setRgbLedColor(HIGH, LOW, LOW);
  delay(DELAY_MS);
  
  // green
  Serial.print("Green: ");
  setRgbLedColor(LOW, HIGH, LOW);
  delay(DELAY_MS);
  
  // blue
  Serial.print("Blue: ");
  setRgbLedColor(LOW, LOW, HIGH);
  delay(DELAY_MS);
  
  // purple
  Serial.print("Purple: ");
  setRgbLedColor(HIGH, LOW, HIGH);
  delay(DELAY_MS);
  
  // turqoise
  Serial.print("Turqoise: ");
  setRgbLedColor(LOW, HIGH, HIGH);
  delay(DELAY_MS);
  
  // yellow
  Serial.print("Yellow: ");
  setRgbLedColor(HIGH, HIGH, LOW);
  delay(DELAY_MS);
  
  // white
  Serial.print("White: ");
  setRgbLedColor(HIGH, HIGH, HIGH);
  delay(DELAY_MS);
  
}

void setRgbLedColor(int red, int green, int blue) {
  if (IS_COMMON_ANODE) {
    red = !red;
    blue = !blue;
    green = !green;
  }

  digitalWrite(RED, red);
  digitalWrite(GREEN, green);
  digitalWrite(BLUE, blue);
}
