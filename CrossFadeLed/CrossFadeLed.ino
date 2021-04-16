const int RED_PIN = 3;
const int GREEN_PIN = 5;
const int BLUE_PIN = 6;
const int DELAY_MS = 20;
const int MAX_COLOR_VALUE = 255;
const int FADE_STEP = 1;
const int COMMON_ANODE = false;

enum RGB {
  RED,
  GREEN,
  BLUE,
  NUM_COLORS
};

int _rgbLedValues[] = {255, 0, 0};
enum RGB _curFadingUp = GREEN;
enum RGB _curFadingDown = RED;

void setup() {
  // put your setup code here, to run once:
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  setColor(HIGH, LOW, LOW);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  _rgbLedValues[_curFadingUp] += FADE_STEP;
  _rgbLedValues[_curFadingDown] -= FADE_STEP;

  if (_rgbLedValues[_curFadingUp] > MAX_COLOR_VALUE) {
    _rgbLedValues[_curFadingUp] = MAX_COLOR_VALUE;
    _curFadingUp = (RGB) ((int)_curFadingUp + 1);
  }

  if (_curFadingUp > (int)BLUE) {
    _curFadingUp = RED;
  }

  if (_rgbLedValues[_curFadingDown] < 0) {
    _rgbLedValues[_curFadingDown] = 0;
    _curFadingDown = (RGB) ((int)_curFadingDown + 1);
  }

  if (_curFadingDown > (int)BLUE) {
    _curFadingDown = RED;
  }

  setColor(_rgbLedValues[RED], _rgbLedValues[GREEN], _rgbLedValues[BLUE]);
  delay(DELAY_MS);
}

void setColor(int red, int green, int blue)
{
  // Necessary for serial plotter
  Serial.print(blue);
  Serial.print(" ");
  Serial.print(red);
  Serial.print(" ");
  Serial.println(green);
  // If a common anode LED, invert values
  if(COMMON_ANODE == true){
    red = MAX_COLOR_VALUE - red;
    green = MAX_COLOR_VALUE - green;
    blue = MAX_COLOR_VALUE - blue;
  }
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);  
}
