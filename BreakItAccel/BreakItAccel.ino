#include <Wire.h>
#include <SPI.h>
#include <Shape.hpp>;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(128, 64, &Wire, 4);
// Declaration for an accelerometer connected to I2C (SDA, SCL pins)
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Load screen text
const char STR_LOADSCREEN_APP_NAME_LINE1[] = "Break It!";
const int LOAD_SCREEN_SHOW_MS = 1000;

// Menu screen
const char STR_PRESS_TO_START[] = "Press A to start";

// Scoreboard
const char STR_SCOREBOARD[] = "Scoreboard";
const char STR_YOURSCORE[] = "Your score: ";
const char STR_CONT[] = "Press A to continue";

// Game text
const char STR_SERVE[] = "Press A to serve";

int _gameModeIdx = 0;
const int DELAY_LOOP_MS = 5;

// Button input
const int LEFT_BTN_PIN = 5;

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

  if (!lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    if (!lis.begin(0x19)) {
      Serial.println("Couldnt start");
      while (1) yield();
    }
  }
  
  randomSeed(analogRead(A5));

  initializeOledAndShowLoadScreen();
  initializeGameEntities();
}

void loop() {
  _display.clearDisplay();
  int btnVal = digitalRead(LEFT_BTN_PIN);
  if (_gameState == NEW_GAME) {
    showMenu();
    if (btnVal == LOW) {
      resetGameEntities();
      _gameState = PLAYING;
      delay(1000);
    }
  } else if (_gameState == PLAYING) {
    playLoop();
    drawStatusBar();
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

  int yText = 28;
  _display.setTextSize(1);
  _display.getTextBounds(STR_PRESS_TO_START, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_PRESS_TO_START);
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

void playLoop() {
  int btnVal = digitalRead(LEFT_BTN_PIN);
  int pSpeed = 0;
  bool hasUnbrokenBrick = false;

  sensors_event_t event;
  lis.getEvent(&event);
  pSpeed = -event.acceleration.x;
  _paddle.setX(_paddle.getX() + pSpeed);

  _paddle.forceInside(0, 0, _display.width(), _display.height());
  _paddle.draw(_display);
  
  if (!_ballActive) {
    // If the ball is not active yet, make it stick with the paddle
    _ball.setCenter(_paddle.getX() + _paddle.getWidth() / 2, _paddle.getY() - _ball.getRadius() - 1);
  } else {
    if (_ball.overlaps(_paddle) && _ball.getBottom() >= _paddle.getTop() && _ball.getYSpeed() > 0) {
      // When the ball overlaps with the paddle, bounce back up
      _ball.reverseYSpeed();
      _ball.setSpeed(constrain(_ball.getXSpeed() + pSpeed, -3, 3), _ball.getYSpeed());
    } else if (_ball.getTop() <= 8 && _ball.getYSpeed() < 0) {
      _ball.reverseYSpeed();
    } else if (_ball.getTop() >= SCREEN_HEIGHT) {
      resetBall();
      _score -= 5;    }
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
  }
}

/*
 * Displays the scoreboard
 */
void scoreboard() {
  int16_t x1, y1;
  uint16_t w, h;
  int yText = 28;
  _display.setTextSize(1);
  _display.getTextBounds(STR_YOURSCORE, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_YOURSCORE);
  _display.print(_score);
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

  _display.display();
  delay(LOAD_SCREEN_SHOW_MS);
  _display.clearDisplay();
}

void drawStatusBar() {
  // Draw accumulated points
  _display.setTextSize(1);
  _display.setCursor(0, 0); // draw points
  _display.print(_score);
}
