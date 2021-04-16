const int LED_OUTPUT_PIN = 3;
const int MAX_ANALOG_OUT = 255; // the max analog output on the Uno is 255
const int DELAY_MS = 5;
int _fadeAmount = 1;
int _currBrightness = 0;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long _lastToggledTimestampMs = 0; // tracks the last time the LED was updated

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Get the current time since the Arduino started our program (in ms)
  unsigned long currentTimestampMs = millis();
  
  // Check to see if the time between now and the last time we toggled
  // the LED has exceeded our delay interval. If so, toggled the LED
  if (currentTimestampMs - _lastToggledTimestampMs >= DELAY_MS) {
    if (_currBrightness < 0 || _currBrightness >= MAX_ANALOG_OUT) {
      _fadeAmount = -_fadeAmount;
    }
    _lastToggledTimestampMs = currentTimestampMs; // store current time as toggle time
    analogWrite(LED_OUTPUT_PIN, _currBrightness);
    _currBrightness += _fadeAmount;
    
  }
  Serial.println(_currBrightness);
}
