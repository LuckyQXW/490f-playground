#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Shape.hpp>;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <Adafruit_LIS3DH.h>
// #include <Adafruit_Sensor.h>
#include <ParallaxJoystick.hpp>;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(128, 64, &Wire, 4);
// Declaration for an accelerometer connected to I2C (SDA, SCL pins)
// Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Load screen text
const char STR_LOADSCREEN_APP_NAME_LINE1[] = "Break It!";
const char STR_LOADSCREEN_APP_NAME_LINE2[] = "490F Edition";
const char STR_LOADSCREEN_CREATOR[] = "By Wen Qiu";
const int LOAD_SCREEN_SHOW_MS = 1000;

// Menu screen
const char STR_SELECT_GAME_MODE[] = "Choose game mode";
const char STR_PRESS_TO_START[] = "Press A to start";
const char STR_BASIC[] = "Basic";
const char STR_ADVANCED[] = "Advanced";

// Scoreboard
const char STR_SCOREBOARD[] = "Scoreboard";
const char STR_YOURSCORE[] = "Your score: ";
const char STR_HIGHSCORE[] = "High score: ";
const char STR_CONT[] = "Press A to continue";

// Game text
const char STR_SERVE[] = "Press A to serve";

int _gameModeIdx = 0;
const int DELAY_LOOP_MS = 5;

// Button input
const int LEFT_BTN_PIN = 5;
const int TONE_OUTPUT_PIN = 6;

// Joystick input
const int JOYSTICK_UPDOWN_PIN = A1;
const int JOYSTICK_LEFTRIGHT_PIN = A0;
const int MAX_ANALOG_VAL = 1023;
const enum JoystickYDirection JOYSTICK_Y_DIR = RIGHT;

// Analog joystick
ParallaxJoystick _analogJoystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);

// Buzzer output
const int COLLISION_TONE_FREQUENCY = 300;
const int LOSE_BALL_TONE_FREQUENCY = 100;
const int PLAY_TONE_DURATION_MS = 200;

// Vibromotor output
const int VIBRO_PIN = 9;
const int MAX_ANALOG_INPUT = 1023;

// Define game components
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIHT = 4;
Rectangle _paddle(SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - PADDLE_HEIHT, PADDLE_WIDTH, PADDLE_HEIHT);
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
const int NUM_ROWS = 3;
const int NUM_BRICK_PER_ROW = 8;
Brick **_bricks;

const int BALL_RADIUS = 2;
Ball _ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 2);
int _ballYSpeed = 1;
bool _ballActive = false;

enum GameState {
  NEW_GAME,
  PLAYING,
  GAME_OVER,
};

GameState _gameState = NEW_GAME;

int _score = 0;

int _highScore = 0;

void setup() {
  Serial.begin(9600);

  pinMode(LEFT_BTN_PIN, INPUT_PULLUP);
  pinMode(TONE_OUTPUT_PIN, OUTPUT);
  pinMode(VIBRO_PIN, OUTPUT);

  /*
  if (!lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    if (!lis.begin(0x19)) {
      Serial.println("Couldnt start");
      while (1) yield();
    }
  }*/
  
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
      delay(1000);
    }
  } else if (_gameState == PLAYING) {
    playLoop(xMovementPixels);
    drawStatusBar();
   if (yMovementPixels > 0) {
      _gameState = GAME_OVER;
    }
  } else if (_gameState == GAME_OVER) {
    scoreboard();
    if (btnVal == LOW) {
      _gameState = NEW_GAME;
      delay(1000);
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
  _display.getTextBounds(STR_PRESS_TO_START, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_PRESS_TO_START);

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
  int brickY = 8;
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
  resetBricks();
  resetPaddle();
  resetBall();
  _score = 0;
}

void resetBricks() {
  for (int i = 0; i < NUM_ROWS * NUM_BRICK_PER_ROW; i++) {
    Brick *brick = _bricks[i];
    brick->setBroken(false);
  }
}

void resetPaddle() {
  _paddle.setX(SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2);
  _paddle.setY(SCREEN_HEIGHT - PADDLE_HEIHT);
}

void resetBall() {
  _ballActive = false;
  _ball.setSpeed(0, 0);
  _ball.setCenter(_paddle.getX() + _paddle.getWidth() / 2, _paddle.getY() - _ball.getRadius() - 1);
}

void playLoop(int leftRightVal) {
  int btnVal = digitalRead(LEFT_BTN_PIN);
  int pSpeed = 0;
  bool hasUnbrokenBrick = false;

  // Decide the actual paddle speed and location based on the joystick value
  if (leftRightVal < 0) {
    pSpeed = -_paddleSpeed;
  } else if (leftRightVal > 0) {
    pSpeed = _paddleSpeed;
  }
  _paddle.setX(_paddle.getX() + pSpeed);

  /*
  if (_gameModeIdx == 0) {
    // Decide the actual paddle speed and location based on the joystick value
    if (leftRightVal < 0) {
      pSpeed = -_paddleSpeed;  
    } else if (leftRightVal > 0) {
      pSpeed = _paddleSpeed;
    }
    _paddle.setX(_paddle.getX() + pSpeed);
  } else {
    sensors_event_t event;
    lis.getEvent(&event);
    pSpeed = event.acceleration.x;
    _paddle.setX(_paddle.getX() + event.acceleration.x);
  }
  */

  _paddle.forceInside(0, 0, _display.width(), _display.height());
  _paddle.draw(_display);
  
  if (!_ballActive) {
    // If the ball is not active yet, make it stick with the paddle
    _ball.setCenter(_paddle.getX() + _paddle.getWidth() / 2, _paddle.getY() - _ball.getRadius() - 1);
    showServeInstruction();
  } else {
    if (_ball.overlaps(_paddle) && _ball.getBottom() >= _paddle.getTop() && _ball.getYSpeed() > 0) {
      // When the ball overlaps with the paddle, bounce back up
      _ball.reverseYSpeed();
      _ball.setSpeed(constrain(_ball.getXSpeed() + pSpeed, -3, 3), _ball.getYSpeed());
    } else if (_ball.getTop() <= 8 && _ball.getYSpeed() < 0) {
      _ball.reverseYSpeed();
    } else if (_ball.getTop() >= SCREEN_HEIGHT) {
      resetBall();
      _score -= 5;
      playLoseBallSound();
    }
    if (_ball.checkXBounce(0, SCREEN_WIDTH - 2 * _ball.getRadius())) {
      _ball.reverseXSpeed();
    }
    _ball.update();
  }
    
  if (!_ballActive && btnVal == LOW) {
    _ballActive = true;
    _ball.setSpeed(random(-5, 5), -_ballYSpeed);
  }

  for (int i = 0; i < NUM_ROWS * NUM_BRICK_PER_ROW; i++) {
    Brick *brick = _bricks[i];
    if (brick->overlaps(_ball) && !brick->isBroken()) {
      brick->setBroken(true);
      _score += 1;
      playBreakBrickSound();
      if (_ball.getCenterY() >= brick->getBottom() && _ball.getYSpeed() < 0) {
        _ball.reverseYSpeed();
      } else if (_ball.getCenterY() < brick->getBottom()) {
        _ball.reverseXSpeed();
      }
    }
    if (!brick->isBroken()) {
      brick->draw(_display);
      hasUnbrokenBrick = true;
    }
  }
  
  _ball.draw(_display);
  if (!hasUnbrokenBrick) {
    _gameState = GAME_OVER;
    int oldHighscore = 0;
    EEPROM.get(0, oldHighscore);
    _highScore = max(oldHighscore, _score);
    EEPROM.put(0, _highScore);
  }
}

void showServeInstruction() {
  int16_t x1, y1;
  uint16_t w, h;
  int yText = 24;
  _display.setTextSize(1);
  _display.getTextBounds(STR_SERVE, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_SERVE);
}

void playBreakBrickSound() {
  tone(TONE_OUTPUT_PIN, COLLISION_TONE_FREQUENCY, PLAY_TONE_DURATION_MS);
}

void playLoseBallSound() {
  tone(TONE_OUTPUT_PIN, LOSE_BALL_TONE_FREQUENCY, PLAY_TONE_DURATION_MS);
  analogWrite(VIBRO_PIN, MAX_ANALOG_INPUT);
  delay(500);
  tone(TONE_OUTPUT_PIN, LOSE_BALL_TONE_FREQUENCY, PLAY_TONE_DURATION_MS);
  analogWrite(VIBRO_PIN, 0);
}

/*
 * Displays the scoreboard
 */
void scoreboard() {
  int16_t x1, y1;
  uint16_t w, h;
  int yText = 0;
  _display.setTextSize(2);
  _display.getTextBounds(STR_SCOREBOARD, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_SCOREBOARD);

  yText += h + 5;
  _display.setTextSize(1);
  _display.getTextBounds(STR_YOURSCORE, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_YOURSCORE);
  _display.print(_score);

  yText += h + 1;
  _display.setTextSize(1);
  _display.getTextBounds(STR_HIGHSCORE, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_HIGHSCORE);
  _display.print(_highScore);

  yText += h + 3;
  _display.setTextSize(1);
  _display.getTextBounds(STR_CONT, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_CONT);
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

/*
 * Displays the laod screen of the game
 */
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

void drawStatusBar() {
  // Draw accumulated points
  _display.setTextSize(1);
  _display.setCursor(0, 0); // draw points
  _display.print(_score);
}
