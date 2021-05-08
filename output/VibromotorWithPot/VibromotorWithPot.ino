const int POT_INPUT_PIN = A0;
const int VIBROMOTOR_OUTPUT_PIN = 3;
const int MAX_ANALOG_INPUT = 1023;

void setup()
{
  Serial.begin(9600);
  pinMode(VIBROMOTOR_OUTPUT_PIN, OUTPUT);
}

void loop()
{
  int analogIn = analogRead(POT_INPUT_PIN);
  int motorOutVal = map(analogIn, 0, MAX_ANALOG_INPUT, 0, 255);
  analogWrite(VIBROMOTOR_OUTPUT_PIN, motorOutVal);
  
  Serial.print(analogIn);
  Serial.print(", ");
  Serial.println(motorOutVal);
}
