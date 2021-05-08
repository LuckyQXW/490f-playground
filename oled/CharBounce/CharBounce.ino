#include <Wire.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(128, 64, &Wire, 4);

// Char global variables
const int _textSize = 3;
const int _textWidth = _textSize * 5;
const int _textHeight = _textSize * 8;
int _x = 0;  // x location of the char
int _y = 0;  // y location of the char
int _xSpeed = 0; // x speed of char (in pixels per frame)
int _ySpeed = 0; // y speed of char (in pixels per frame)

void setup() {
  // Initialize the display
  _display.begin(SSD1306_SWITCHCAPVCC, 0x3D);

  // Gets a random long between min and max - 1
  // https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
  _xSpeed = random(1, 4);
  _ySpeed = random(1, 4);
  _x = SCREEN_WIDTH / 2;
  _y = SCREEN_HEIGHT / 2;
}

void loop() {
  // Clear the display
  _display.clearDisplay();
  
  // Update char based on speed location
  _x += _xSpeed;
  _y += _ySpeed;

  // Check for char bounce. First check for going off left or right side of screen
  if(_x <= 0 || _x + _textWidth >= _display.width()){
    _xSpeed = _xSpeed * -1; // reverse x direction
  }
  
  // Now check for bouncing on floor or ceiling
  if(_y <= 0 || _y + _textHeight >= _display.height()){
    _ySpeed = _ySpeed * -1; // reverse y direction
  }

  // Draw char
  _display.drawChar(_x, _y, 'Q', SSD1306_WHITE, SSD1306_BLACK, _textSize);
  
  // Render buffer to screen
  _display.display();
}
