#include "src/RGBConverter/RGBConverter.h"

const int RGB_RED_PIN = 3;
const int RGB_GREEN_PIN = 5;
const int RGB_BLUE_PIN = 6;
const int DELAY_MS = 50;
const int MAX_RGB_VALUE = 255;
const int FADE_STEP = 1;
const int COMMON_ANODE = false;

RGBConverter _rgbConverter;
float _hue = 0;
float _saturation = 1;
float _lightness = 0.2;
float _step = 0.05f;
float _hueStep = 0.2f;

void setup() {
  // put your setup code here, to run once:
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  byte rgb[3];
  _rgbConverter.hslToRgb(_hue, _saturation, _lightness, rgb);

  /*
  Serial.print("hue=");
  Serial.print(_hue);
  Serial.print(" r=");
  Serial.print(rgb[0]);
  Serial.print(" g=");
  Serial.print(rgb[1]);
  Serial.print(" b=");
  Serial.println(rgb[2]);
  */
  
  setColor(rgb[0], rgb[1], rgb[2]); 

  // update saturation based on step size
  _saturation -= _step;

  // if saturation reaches 0, change to a different color and invert steps
  if(_saturation < 0.0){
    _saturation = 0.0;
    _step = -_step;
    _hue += _hueStep;
    // hue ranges between 0-1, so if > 1, reset to 0
    if (_hue > 1.0) {
      _hue = 0;
    }
  } else if (_saturation > 1.0) {
    _saturation = 1.0;
    _step = -_step;
  }
  Serial.print("saturation=");
  Serial.println(_saturation);

  delay(DELAY_MS);
}

void setColor(int red, int green, int blue)
{
  // If a common anode LED, invert values
  if(COMMON_ANODE == true){
    red = MAX_RGB_VALUE - red;
    green = MAX_RGB_VALUE - green;
    blue = MAX_RGB_VALUE - blue;
  }
  analogWrite(RGB_RED_PIN, red);
  analogWrite(RGB_GREEN_PIN, green);
  analogWrite(RGB_BLUE_PIN, blue);  
}
