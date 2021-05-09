#include <Wire.h>
#include <SPI.h>
#include <ParallaxJoystick.hpp>;
#include <PushButton.h>
#include <Shape.hpp>;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char STR_GAME_NAME[] = "Dino Run!";
const char STR_PRESS_UP_TO_JUMP[] = "Press up to jump";
const char STR_GAME_OVER[] = "Game Over!";

const int DELAY_LOOP_MS = 5; // change to slow down how often to read and graph value

const int JOYSTICK_UPDOWN_PIN = A1;
const int JOYSTICK_LEFTRIGHT_PIN = A0;

const int MAX_ANALOG_VAL = 1023;
const enum JoystickYDirection JOYSTICK_Y_DIR = RIGHT;

// Analog joystick
ParallaxJoystick _analogJoystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);

// Digital buttons (joystick)
const int UP_BUTTON_INPUT_PIN = 6;
const int DOWN_BUTTON_INPUT_PIN = 7;
const int LEFT_BUTTON_INPUT_PIN = 5;
const int RIGHT_BUTTON_INPUT_PIN = 4;

PushButton jump(LEFT_BUTTON_INPUT_PIN);

class Rock : public Rectangle {
  protected:
    bool _hasPassedDino = false;
    bool _hasCollidedDino = false;

  public:
    Rock(int x, int y, int width, int height) : Rectangle(x, y, width, height)
    {
    }

    bool getHasPassedDino() {
      return _hasPassedDino;
    }

    bool setHasPassedDino(bool hasPassedDino) {
      _hasPassedDino = hasPassedDino;
    }

    bool getHasCollidedDino() {
      return _hasCollidedDino;
    }

    bool setHasCollidedDino(bool hasCollidedDino) {
      _hasCollidedDino = hasCollidedDino;
    }
};

class Dino : public Rectangle {
  public:
    Dino(int x, int y, int width, int height) : Rectangle(x, y, width, height)
    {
    }
};

const int DINO_HEIGHT = 20;
const int DINO_WIDTH = 20;
const int NUM_ROCKS = 3;

const int MIN_ROCK_WIDTH = 5;
const int MAX_ROCK_WIDTH = 10; // in pixels

const int MIN_ROCK_HEIGHT = 5;
const int MAX_ROCK_HEIGHT = 10; // in pixels

const int MIN_ROCK_X_SPACING_DISTANCE = DINO_WIDTH * 4; // in pixels
const int MAX_ROCK_X_SPACING_DISTANCE = 100; // in pixels

Dino _dino(5, SCREEN_HEIGHT / 2, DINO_WIDTH, DINO_HEIGHT);

Rock _rocks[NUM_ROCKS] = { Rock(0, 0, 0, 0),
                           Rock(0, 0, 0, 0),
                           Rock(0, 0, 0, 0)
                         };

int _points = 0;
int _lives = 3;

enum GameState {
  NEW_GAME,
  PLAYING,
  GAME_OVER,
};

GameState _gameState = NEW_GAME;
unsigned long _gameOverTimestamp = 0;
unsigned long _startGameTimestamp = 0;
const int IGNORE_INPUT_AFTER_GAME_OVER_MS = 500; //ignores input for 500ms after game over

// 'dino', 20x20px
const unsigned char dino [] PROGMEM = {
  0x00, 0x0f, 0xe0, 0x00, 0x17, 0xf0, 0x00, 0x1f, 0xf0, 0x00, 0x1f, 0xf0, 0x00, 0x1f, 0xf0, 0x00, 
  0x1e, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x3c, 0x00, 0x40, 0xfc, 0x00, 0x61, 0xff, 0x00, 0x73, 0xfc, 
  0x00, 0x7f, 0xfc, 0x00, 0x3f, 0xfc, 0x00, 0x1f, 0xf8, 0x00, 0x07, 0xf0, 0x00, 0x07, 0xe0, 0x00, 
  0x03, 0x20, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 0x00
};

int _dinoX = 0;
int _dinoY = 44;

int _rockSpeed = 2;
int _gravity = 2;
boolean _jumping = false;
boolean _crouching = false;

void setup() {
  Serial.begin(9600);
  pinMode(UP_BUTTON_INPUT_PIN, INPUT_PULLUP);
  jump.setActiveLogic(LOW);
  randomSeed(analogRead(A5));
  initializeGameEntities();
  initializeOledAndShowStartupScreen();
}

void loop() {
  _display.clearDisplay();
  drawStatusBar();
  if (_gameState == NEW_GAME || _gameState == GAME_OVER) {
    nonGamePlayLoop();
  } else if (_gameState == PLAYING) {
    gamePlayLoop();
  }
  // Render buffer to screen
  _display.display();

  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}

void nonGamePlayLoop() {
  jump.update();
  int16_t x1, y1;
  uint16_t w, h;
  if (_gameState == NEW_GAME) {
    _display.getTextBounds(STR_PRESS_UP_TO_JUMP, 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() / 2 - w / 2, 15);
    _display.print(STR_PRESS_UP_TO_JUMP);

    if (jump.isClicked()) {
      _gameState = PLAYING;
      _startGameTimestamp = millis();
    }
  } else if (_gameState == GAME_OVER) {
    _display.setTextSize(2);
    _display.getTextBounds(STR_GAME_OVER, 0, 0, &x1, &y1, &w, &h);
    int yText = 15;
    _display.setCursor(_display.width() / 2 - w / 2, yText);
    _display.print(STR_GAME_OVER);

    yText = yText + h + 2;
    _display.setTextSize(1);
    _display.getTextBounds(STR_PRESS_UP_TO_JUMP, 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() / 2 - w / 2, yText);
    _display.print(STR_PRESS_UP_TO_JUMP);

    // We ignore input a bit after game over so that user can see end game screen
    // and not accidentally start a new game
    if (jump.isClicked() && millis() - _gameOverTimestamp >= IGNORE_INPUT_AFTER_GAME_OVER_MS) {
      // if the current state is game over, need to reset
      initializeGameEntities();
      _gameState = PLAYING;
      _startGameTimestamp = millis();
    }
  }

  _display.drawBitmap(_dino.getX(), _dino.getY(), dino, 20, 20, WHITE);
}

void initializeGameEntities() {
  _points = 0;
  _lives = 3;
  
  _dino.setX(10);
  _dino.setY(44);
  _dino.forceInside(0, 0, _display.width(), _display.height());

  const int minStartXRockLocation = _display.width() / 2;
  int lastRockX = minStartXRockLocation;
  for (int i = 0; i < NUM_ROCKS; i++) {

    int rockX = lastRockX + random(MIN_ROCK_X_SPACING_DISTANCE, MAX_ROCK_X_SPACING_DISTANCE);
    int rockWidth = random(MIN_ROCK_WIDTH, MAX_ROCK_WIDTH);
    int rockHeight = random(MIN_ROCK_HEIGHT, MAX_ROCK_HEIGHT);
    int rockY = _display.height() - rockHeight;
    _rocks[i].setLocation(rockX, rockY);
    _rocks[i].setDimensions(rockWidth, rockHeight);
    _rocks[i].setDrawFill(false);
    _rocks[i].setHasPassedDino(false);
    _rocks[i].setHasCollidedDino(false);
    lastRockX = _rocks[i].getRight();
  }
}

void gamePlayLoop() {
  // dino jump movement
  jump.update();
  // Read analog joystick to control player
  _dino.setY(_dino.getY() + _gravity);
  if (jump.isClicked()) {
    _jumping = true;
  }

  if (_jumping && _dino.getY() >= 20) {
    _dino.setY(_dino.getY() - 4);
  } else {
    _jumping = false;
  }
  
  _dino.forceInside(0, 0, _display.width(), _display.height());

  int xMaxRight = 0;
  
  for (int i = 0; i < NUM_ROCKS; i++) {
    _rocks[i].setX(_rocks[i].getX() - _rockSpeed);

    _rocks[i].draw(_display);

    // Check if the bird passed by the pipe
    if (_rocks[i].getRight() < _dinoX) {

      // If we're here, the bird has passed the pipe. Check to see
      // if we've marked it as passed yet. If not, then increment the score!
      if (!_rocks[i].getHasPassedDino() && !_rocks[i].getHasCollidedDino()) {
        _points++;
        _rocks[i].setHasPassedDino(true);
      }
    }

    // xMaxRight is used to track future placements of pipes once
    // they go off the left part of the screen
    if (xMaxRight < _rocks[i].getRight()) {
      xMaxRight = _rocks[i].getRight();
    }

    // Check for collisions and end of game
    if (_rocks[i].overlaps(_dino) && !_rocks[i].getHasCollidedDino()) {
      _rocks[i].setDrawFill(true);
      _rocks[i].setHasCollidedDino(true);
      _lives--;
      if (_lives == 0) {
        _gameOverTimestamp = millis();
        _gameState = GAME_OVER;
      }
    } else if (_rocks[i].getHasCollidedDino()) {
      _rocks[i].setDrawFill(true);
    } else {
      _rocks[i].setDrawFill(false);
    }
  }

  // Check for rocks that have gone off the screen to the left
  // and reset them to off the screen on the right
  xMaxRight = max(xMaxRight, _display.width());
  for (int i = 0; i < NUM_ROCKS; i++) {
    if (_rocks[i].getRight() < 0) {
      int rockX = xMaxRight + random(MIN_ROCK_X_SPACING_DISTANCE, MAX_ROCK_X_SPACING_DISTANCE);

      int rockWidth = random(MIN_ROCK_WIDTH, MAX_ROCK_WIDTH);
      int rockHeight = random(MIN_ROCK_HEIGHT, MAX_ROCK_HEIGHT);
      int rockY = _display.height() - rockHeight;
      _rocks[i].setLocation(rockX, rockY);
      _rocks[i].setDimensions(rockWidth, rockHeight);
      _rocks[i].setHasPassedDino(false);
      _rocks[i].setHasCollidedDino(false);
      xMaxRight = _rocks[i].getRight();
    }
  }
    
  _display.drawBitmap(_dino.getX(), _dino.getY(), dino, 20, 20, WHITE);
  _points = (int) (millis() - _startGameTimestamp) / 1000;
}

void drawStatusBar() {
  // Draw accumulated points
  _display.setTextSize(1);
  _display.setCursor(0, 0); // draw points
  _display.print(_points);
  
  int lastHeartX = 24;
  for (int i = 0; i < _lives; i++) {
    _display.setCursor(lastHeartX, 0);
    _display.write(3);
    lastHeartX += 10;
  }
}

/**
 * Call this from setup() to initialize the OLED screen
 */
void initializeOledAndShowStartupScreen(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  int16_t x1, y1;
  uint16_t w, h;
  // Clear the buffer
  _display.clearDisplay();
  _display.setTextSize(2);
  _display.setTextColor(WHITE, BLACK);
  _display.getTextBounds(STR_GAME_NAME, 0, 0, &x1, &y1, &w, &h);
  int yText = 15;
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_GAME_NAME);
  _display.display();
  delay(3000);
  _display.clearDisplay();
}
