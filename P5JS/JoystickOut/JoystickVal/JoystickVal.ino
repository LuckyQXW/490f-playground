#include <ParallaxJoystick.hpp>;

// Joystick input
const int JOYSTICK_UPDOWN_PIN = A1;
const int JOYSTICK_LEFTRIGHT_PIN = A0;
const int MAX_ANALOG_VAL = 1023;
const enum JoystickYDirection JOYSTICK_Y_DIR = LEFT;

int _val = 0;

// Analog joystick
ParallaxJoystick _analogJoystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  _analogJoystick.read();
  int leftRightVal = _analogJoystick.getLeftRightVal();
  int xMovementPixels = map(leftRightVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -2, 3);
  if (_val != xMovementPixels) {
    Serial.println(xMovementPixels);
    _val = xMovementPixels;
  }
  delay(5);
}
