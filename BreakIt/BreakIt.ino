#include <Wire.h>
#include <SPI.h>
#include <Shape.hpp>;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <ParallaxJoystick.hpp>;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(128, 64, &Wire, 4);
// Declaration for an accelerometer connected to I2C (SDA, SCL pins)
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Load screen text
const char STR_LOADSCREEN_APP_NAME_LINE1[] = "Break It!";
const char STR_LOADSCREEN_APP_NAME_LINE2[] = "490F Edition";
const char STR_LOADSCREEN_CREATOR[] = "By Wen Qiu";
const int LOAD_SCREEN_SHOW_MS = 1000;

// Menu screen
const char STR_SELECT_GAME_MODE[] = "Choose game mode";
const char STR_DOUBLE_CLICK_TO_START[] = "Hold to start";
const char STR_BASIC[] = "Basic";
const char STR_ADVANCED[] = "Advanced";

// Scoreboard
const char STR_SCOREBOARD[] = "Scoreboard";

int _gameModeIdx = 0;
const int DELAY_LOOP_MS = 5;

// Button input
const int LEFT_BTN_PIN = 5;
const int RIGHT_BTN_PIN = 6;

// Joystick input
const int JOYSTICK_UPDOWN_PIN = A1;
const int JOYSTICK_LEFTRIGHT_PIN = A0;
const int MAX_ANALOG_VAL = 1023;
const enum JoystickYDirection JOYSTICK_Y_DIR = RIGHT;

// Analog joystick
ParallaxJoystick _analogJoystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);

// Define game components
class Paddle : public Rectangle {
  public:
    Paddle(int x, int y, int width, int height) : Rectangle(x, y, width, height)
    {
    }
};
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIHT = 4;
Paddle _paddle(SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - PADDLE_HEIHT, PADDLE_WIDTH, PADDLE_HEIHT);
int _paddleSpeed = 3;

class Brick : public Rectangle {
  protected:
    bool _broken = false;

  public:
    Brick(int x, int y, int width, int height) : Rectangle(x, y, width, height)
    {
    }

    bool isBroken() {
      return _broken;
    }

    bool setBroken(bool broken) {
      _broken = broken;
    }
};
const int BRICK_WIDTH = 15;
const int BRICK_HEIGHT = 4;
const int BRICK_GAP = 1;
const int NUM_ROWS = 2;
const int NUM_BRICK_PER_ROW = 8;
Brick **_bricks;

class BrickBall : public Ball {
  protected:
    bool _active = false;

  public:
    BrickBall(int xCenter, int yCenter, int radius) : Ball(xCenter, yCenter, radius) {
    }

    bool isActive() {
      return _active;
    }

    bool setActive(bool active) {
      _active = active;
    }
};
const int BALL_RADIUS = 2;
BrickBall _ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 2);
int _ballYSpeed = 1;

enum GameState {
  NEW_GAME,
  PLAYING,
  GAME_OVER,
};

GameState _gameState = NEW_GAME;

void setup() {
  Serial.begin(9600);

  pinMode(LEFT_BTN_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BTN_PIN, INPUT_PULLUP);
  
  randomSeed(analogRead(A5));

  initializeOledAndShowLoadScreen();
  initializeGameEntities();
}

void loop() {
  _display.clearDisplay();
  // Read analog joystick to control player ball
  _analogJoystick.read();
  int upDownVal = _analogJoystick.getUpDownVal();
  int leftRightVal = _analogJoystick.getLeftRightVal();
  int yMovementPixels = map(upDownVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -4, 5);
  int xMovementPixels = map(leftRightVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -10, 11);
  int btnVal = digitalRead(LEFT_BTN_PIN);
  if (_gameState == NEW_GAME) {
    if (xMovementPixels < 0) {
      _gameModeIdx = max(0, _gameModeIdx - 1);
    } else if (xMovementPixels > 0) {
      _gameModeIdx = min(1, _gameModeIdx + 1);
    }
    showMenu();
    if (btnVal == LOW) {
      resetGameEntities();
      _gameState = PLAYING;
    }
  } else if (_gameState == PLAYING) {
    playLoop(xMovementPixels);
   if (yMovementPixels > 0) {
      _gameState = GAME_OVER;
    }
  } else if (_gameState == GAME_OVER) {
    scoreboard();
    if (yMovementPixels < 0) {
      _gameState = NEW_GAME;
    }
  }
  
  _display.display();
  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}

void showMenu() {
  // Show select game mode screen
  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);
  int16_t x1, y1;
  uint16_t w, h;
  _display.setTextSize(1);

  int yText = 0;

  _display.setTextSize(1);
  _display.getTextBounds(STR_SELECT_GAME_MODE, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_SELECT_GAME_MODE);

  yText = yText + h + 1;
  _display.getTextBounds(STR_DOUBLE_CLICK_TO_START, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_DOUBLE_CLICK_TO_START);

  if (_gameModeIdx == 0) {
    _display.setTextColor(BLACK, WHITE);
  }
  _display.setTextSize(1);
  yText = yText + h + 5;
  _display.getTextBounds(STR_BASIC, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(0, yText);
  _display.print(STR_BASIC);

  _display.setTextColor(WHITE, BLACK);
  if (_gameModeIdx == 1) {
    _display.setTextColor(BLACK, WHITE);
  }
  _display.getTextBounds(STR_ADVANCED, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() - w, yText);
  _display.print(STR_ADVANCED);
}

void initializeGameEntities() {
  // paddle set up
  _paddle.setDrawFill(true);

  // ball set up
  _ball.setDrawFill(true);
  _ball.setCenter(_paddle.getX() + _paddle.getWidth() / 2, _paddle.getY() - _ball.getRadius() - 1);

  // bricks set up
  _bricks = new Brick*[NUM_ROWS * NUM_BRICK_PER_ROW];
  int brickY = 0;
  int brickX = 0;
  for (int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < NUM_BRICK_PER_ROW; j++) {
      _bricks[i * NUM_BRICK_PER_ROW + j] = new Brick(brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT);
      _bricks[i * NUM_BRICK_PER_ROW + j]->setDrawFill(true);
      brickX += BRICK_GAP + BRICK_WIDTH;
    }
    brickX = 0;
    brickY += BRICK_GAP + BRICK_HEIGHT;
  }
}

void resetGameEntities() {
  for (int i = 0; i < NUM_ROWS * NUM_BRICK_PER_ROW; i++) {
    Brick *brick = _bricks[i];
    brick->setBroken(false);
  }

  _paddle.setX(SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2);
  _paddle.setY(SCREEN_HEIGHT - PADDLE_HEIHT);

  _ball.setActive(false);
  _ball.setSpeed(0, 0);
  _ball.setCenter(_paddle.getX() + _paddle.getWidth() / 2, _paddle.getY() - _ball.getRadius() - 1);
}

void playLoop(int leftRightVal) {
  int btnVal = digitalRead(RIGHT_BTN_PIN);
  int pSpeed = 0;
  bool hasUnbrokenBrick = false;
  if (leftRightVal < 0) {
    pSpeed = -_paddleSpeed;  
  } else if (leftRightVal > 0) {
    pSpeed = _paddleSpeed;
  }
  
  _paddle.setX(_paddle.getX() + pSpeed);
  _paddle.forceInside(0, 0, _display.width(), _display.height());
  _paddle.draw(_display);
  
  if (!_ball.isActive()) {
    _ball.setCenter(_paddle.getX() + _paddle.getWidth() / 2, _paddle.getY() - _ball.getRadius() - 1);
  } else {
    if (_ball.overlaps(_paddle)) {
      _ball.reverseYSpeed();
      _ball.setSpeed(constrain(_ball.getXSpeed() + pSpeed, -3, 3), _ball.getYSpeed());
    } else if (_ball.checkYBounce(0, SCREEN_HEIGHT - 2 * _ball.getRadius() + 1)) {
      _ball.reverseYSpeed();
    }
    if (_ball.checkXBounce(0, SCREEN_WIDTH - 2 * _ball.getRadius())) {
      _ball.reverseXSpeed();
    }
    _ball.update();
  }
    
  if (!_ball.isActive() && btnVal == LOW) {
    _ball.setActive(true);
    _ball.setSpeed(random(-5, 5), -_ballYSpeed);
  }

  for (int i = 0; i < NUM_ROWS * NUM_BRICK_PER_ROW; i++) {
    Brick *brick = _bricks[i];
    if (brick->overlaps(_ball) && !brick->isBroken()) {
      brick->setBroken(true);
      _ball.reverseYSpeed();
    }
    if (!brick->isBroken()) {
      brick->draw(_display);
      hasUnbrokenBrick = true;
    }
  }
  
  _ball.draw(_display);
  if (!hasUnbrokenBrick) {
    _gameState = GAME_OVER;
  }
}

void scoreboard() {
  int16_t x1, y1;
  uint16_t w, h;
  int yText = 0;
  _display.setTextSize(2);
  _display.getTextBounds(STR_SCOREBOARD, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_SCOREBOARD);
}
/**
 * Call this from setup() to initialize the OLED screen
 */
void initializeOledAndShowLoadScreen(){
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  showLoadScreen();
}

void showLoadScreen() {
  // Clear the buffer
  _display.clearDisplay();

  // Show load screens
  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);

  int16_t x1, y1;
  uint16_t w, h;
  _display.setTextSize(1);

  int yText = 16;

  _display.setTextSize(2);
  _display.getTextBounds(STR_LOADSCREEN_APP_NAME_LINE1, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_LOADSCREEN_APP_NAME_LINE1);

  _display.setTextSize(1);
  yText = yText + h + 1;
  _display.getTextBounds(STR_LOADSCREEN_APP_NAME_LINE2, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_LOADSCREEN_APP_NAME_LINE2);

  yText = yText + h + 1;
  _display.getTextBounds(STR_LOADSCREEN_CREATOR, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_LOADSCREEN_CREATOR);

  _display.display();
  delay(LOAD_SCREEN_SHOW_MS);
  _display.clearDisplay();
  _display.setTextSize(1);
}
