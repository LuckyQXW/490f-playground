#include "src/RGBConverter/RGBConverter.h"
#include <PushButton.h>

// Input pins
const int BTN_PIN = 2;
const int PHOTO_PIN = A0;
const int VARIABLE_PIN = A1;
const int MICROPHONE_PIN = A2;

// Output pins
const int RED_PIN = 3;
const int GREEN_PIN = 5;
const int BLUE_PIN = 6;

// CrossFade
const int DELAY_MS = 50;
const int MAX_RGB_VALUE = 255;
const int FADE_STEP = 1;
const int COMMON_ANODE = false;

RGBConverter _rgbConverter;
float _hue = 0;
float _saturation = 1;
float _lightness = 0.5;
float _step = 0.001f;

// Photocell input to lightness conversion ratio
const float PHOTO_TO_LIGHTNESS = 0.5/900;

// Lo-fi variable resistor values
float _varHue[3] = {0.0, 1.0/6, 1.0/3};  // corresponds to red, yellow, and green in hue value
int _lastSelectedColor = 0;

// Tap to sound
const int SAMPLE_WINDOW = 50; // Sample window width in mS (50 mS = 20Hz)
const float TAP_THRESHOLD = 2.5;  // Min voltage for registering a tap
unsigned int _sample;

// Button control
PushButton switchBtn(BTN_PIN);
int _mode = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(PHOTO_PIN, INPUT);
  
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  switchBtn.setActiveLogic(LOW);
  Serial.begin(9600);
}

void loop() {
  switchBtn.update();
  // Check for mode switch and update mode if necessary
  if (switchBtn.isClicked()) {
    _mode++;
    _mode = _mode % 3;
  }

  // Update lightness based on photocell value
  int lightVal = analogRead(PHOTO_PIN);
  _lightness = 0.5 - lightVal * PHOTO_TO_LIGHTNESS;
  if (_mode == 0) {
    // Cross fade mode
    _step = 0.001f;
    updateColor();
    delay(DELAY_MS);
  } else if (_mode == 1) {
    // Variable resistance mode
    int varInput = analogRead(VARIABLE_PIN);
    if (varInput > 0) {
      int index = varInput / 100;
      if (index >= 0 && index <= 2) {
        _lastSelectedColor = index;
      }
      setColorInHsl(_varHue[_lastSelectedColor]);
    }
    delay(DELAY_MS);
  } else {
    // Tap mode
    if (getTap()) {
      // Change hue with bigger increments
      _step = 0.1f;
      updateColor();
    }
  }
}

/**
 * Updates the color of the RGB LED with existing hue, saturation, and lightness value
 * Adapted from https://github.com/makeabilitylab/arduino/tree/master/Basics/analogWrite/CrossFadeRGB
 */
void updateColor() {
  byte rgb[3];
  _rgbConverter.hslToRgb(_hue, _saturation, _lightness, rgb);
  setColor(rgb[0], rgb[1], rgb[2]); 

  _hue += _step;

  // hue ranges between 0-1, so if > 1, reset to 0
  if(_hue > 1.0){
    _hue = 0;
  }
}

/**
 * setColor takes in values between 0 - 255 for the amount of red, green, and blue, respectively
 * where 255 is the maximum amount of that color and 0 is none of that color. You can illuminate
 * all colors by intermixing different combinations of red, green, and blue. 
 * 
 * This function is based on https://gist.github.com/jamesotron/766994
 * 
 */
void setColor(int red, int green, int blue)
{
  // If a common anode LED, invert values
  if(COMMON_ANODE == true){
    red = MAX_RGB_VALUE - red;
    green = MAX_RGB_VALUE - green;
    blue = MAX_RGB_VALUE - blue;
  }
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);  
}

/**
 * Takes in a hue ratio and sets color to that ratio
 */
void setColorInHsl(float ratio) {
  byte rgb[3];
  _rgbConverter.hslToRgb(ratio, _saturation, _lightness, rgb);
  setColor(rgb[0], rgb[1], rgb[2]); 
}

/**
 * Returns true if a tap has been registered, false otherwise
 * Adapted from Adafruit tutorial: https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
 */
boolean getTap() {
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  
  // collect data for 50 mS
  while (millis() - startMillis < SAMPLE_WINDOW) {
    _sample = analogRead(MICROPHONE_PIN);
    if (_sample < 1024)  // toss out spurious readings
    {
       if (_sample > signalMax)
       {
          signalMax = _sample;  // save just the max levels
       }
       else if (_sample < signalMin)
       {
          signalMin = _sample;  // save just the min levels
       }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
  return volts > TAP_THRESHOLD;
}
