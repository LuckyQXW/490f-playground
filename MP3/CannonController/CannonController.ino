#include <Shape.hpp>;

#include <Adafruit_SSD1306.h>
#include <ParallaxJoystick.hpp>;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(128, 64, &Wire, 4);

const long BAUD_RATE = 115200;
const int SIZE = 20;
const int SHAPE_OFFSET = 2;
const int NUM_SHAPES_PER_WAVE = 5;
const int MAX_SEQUENCE_LENGTH = 15;

// Graphics indices
int _indices[NUM_SHAPES_PER_WAVE];

// Joystick input
const int JOYSTICK_UPDOWN_PIN = A0;
const int JOYSTICK_LEFTRIGHT_PIN = A1;
const int MAX_ANALOG_VAL = 1023;
const enum JoystickYDirection JOYSTICK_Y_DIR = LEFT;
ParallaxJoystick _analogJoystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);
int _val = 0;

// Vibromotor output
const int VIBRO_PIN = 9;
const int MAX_ANALOG_INPUT = 1023;
const int VIBRATION_DELAY = 500;
long vibrationStart = 0;

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(VIBRO_PIN, OUTPUT);
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
}

void loop() {
  _display.clearDisplay();
  checkVibrationEnd();
  drawSequence();
  // Check to see if there is any incoming serial data
  if(Serial.available() > 0){
    // Echo the data back on serial (for debugging purposes)
    String rcvdSerialData = Serial.readStringUntil('\n');
    if (rcvdSerialData.length() > 1) {
      // Next wave sequence _,_,_,_,_
      // -1: none
      // 0: circle
      // 1: triangle
      // 2: square
      updateNextSequence(rcvdSerialData);
    } else {
      // enemy breach signal
      playBreachVibration();
    }
  }
  _display.display();

  // Send in joystick value if it has changed
  _analogJoystick.read();
  int leftRightVal = _analogJoystick.getLeftRightVal();
  int xMovementPixels = map(leftRightVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -1, 2);
  if (_val != xMovementPixels) {
    Serial.println(xMovementPixels);
    _val = xMovementPixels;
  }
}

void updateNextSequence(String inputData) {
  char buf[MAX_SEQUENCE_LENGTH];
  inputData.toCharArray(buf, inputData.length() + 1);
  int n = sscanf(buf, "%d,%d,%d,%d,%d", &_indices[0], &_indices[1], &_indices[2], &_indices[3], &_indices[4]);
}

void drawSequence() {
  int x = 0;
  int y = 5;
  for (int i = 0; i < NUM_SHAPES_PER_WAVE; i++) {
    drawShape(_indices[i], x, y);
    x += 26;
  }
}

void drawShape(int idx, int x, int y) {
  switch(idx) {
    case 0:
      _display.fillCircle(x + SIZE / 2 + SHAPE_OFFSET, y + SIZE / 2, SIZE / 2, SSD1306_WHITE);
      break;
    case 1:
      _display.fillRect(x + SHAPE_OFFSET, y, SIZE, SIZE, SSD1306_WHITE);
      break;
    case 2:
      _display.fillTriangle(x + SIZE / 2 + SHAPE_OFFSET, y, x + SHAPE_OFFSET, y + SIZE, x + SIZE + SHAPE_OFFSET, y + SIZE, SSD1306_WHITE);
      break;
    default:
      break;
  }
}

void playBreachVibration() {
  vibrationStart = millis();
  analogWrite(VIBRO_PIN, MAX_ANALOG_INPUT);
}

void checkVibrationEnd() {
  if (millis() - vibrationStart >= VIBRATION_DELAY) {
    analogWrite(VIBRO_PIN, 0);
  }
}
