#include <Wire.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(128, 64, &Wire, 4);

// char global variables
const int _textSize = 3;
const int _textWidth = _textSize * 5;
const int _textHeight = _textSize * 8;
int _x = 0;  // x location of the char
int _y = 0;  // y location of the char

// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

void setup() {
  Serial.begin(9600);

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }

  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  // Initialize the display
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Gets a random long between min and max - 1
  // https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
  _xSpeed = random(1, 4);
  _ySpeed = random(1, 4);
  _x = SCREEN_WIDTH / 2;
  _y = SCREEN_HEIGHT / 2;
}

void loop() {

  sensors_event_t event;
  lis.getEvent(&event);
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y);
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z);
  Serial.println(" m/s^2 ");

  Serial.println();
  
  // Clear the display
  _display.clearDisplay();
  
  // Update character based on speed location
  _x -= event.acceleration.x;
  _y += event.acceleration.y;

  // Constraint the character position on screen
  if (_x <= 0) {
    _x = 0;
  } else if (_x + _textWidth >= _display.width()) {
    _x = _display.width() - _textWidth;
  }

  if (_y <= 0) {
    _y = 0;
  } else if (_y + _textHeight >= _display.height()) {
    _y = _display.height() - _textHeight;
  }

  // Draw character
  _display.drawChar(_x, _y, 'Q', SSD1306_WHITE, SSD1306_BLACK, _textSize);
  
  // Render buffer to screen
  _display.display();
}
