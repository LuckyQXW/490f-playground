#include <Wire.h>
#include <SPI.h>
#include <Preferences.h>

// From makability lab arduino library
// https://github.com/makeabilitylab/arduino/tree/master/MakeabilityLab_Arduino_Library
#include <Shape.hpp>
#include <Tone32.hpp>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <ParallaxJoystick.hpp>;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// https://github.com/bhagman/Tone#musical-notes
#define KEY_C 262  // 261.6256 Hz (middle C)
#define KEY_D 294  // 293.6648 Hz
#define KEY_E 330  // 329.6276 Hz
#define KEY_F 350  // 349.2282 Hz
#define KEY_G 392  // 391.9954 Hz

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
const char STR_PRESS_TO_START[] = "Press Button to start";
const char STR_BASIC[] = "Basic";
const char STR_ADVANCED[] = "Advanced";

// Scoreboard
const char STR_SCOREBOARD[] = "Scoreboard";
const char STR_YOURSCORE[] = "Your score: ";
const char STR_HIGHSCORE[] = "High score: ";
const char STR_CONT[] = "Press Button to menu";

// Game text
const char STR_SERVE[] = "Press Button to serve";

// Button input
const int LEFT_BTN_PIN = 15;

// Joystick input
const int JOYSTICK_UPDOWN_PIN = A9;
const int JOYSTICK_LEFTRIGHT_PIN = A10;
const int MAX_ANALOG_VAL = 3800;
const enum JoystickYDirection JOYSTICK_Y_DIR = RIGHT;

// Analog joystick
ParallaxJoystick _analogJoystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);

// Buzzer output
const int TONE_OUTPUT_PIN = 14;
const int COLLISION_TONE_FREQUENCY = 300;
const int LOSE_BALL_TONE_FREQUENCY = 100;
const int PLAY_TONE_DURATION_MS = 200;

const int PWM_CHANNEL = 0;    // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_FREQ = 500;     // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
const int PWM_RESOLUTION = 8; // We'll use same resolution as Uno (8 bits, 0-255) but ESP32 can go up to 16 bits 
Tone32 _tone32(TONE_OUTPUT_PIN, PWM_CHANNEL);
const int WIN_DELAY = 100;
unsigned long _lastWin = 0;
bool _playedWinGameSound = true;

// Vibromotor output
const int VIBRO_PIN = 32;
const int VIBRATION_INTERVAL = 300;
int _vibrationStart = 0;

// Define game components
// Paddle
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIHT = 4;
Rectangle _paddle(SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - PADDLE_HEIHT, PADDLE_WIDTH, PADDLE_HEIHT);
int _paddleSpeed = 3;

// Brick
class Brick : public Rectangle {
  protected:
    bool _broken = false;
    bool _hasPowerUp = false;

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

    void setHasPowerUp(bool hasPowerUp) {
      _hasPowerUp = hasPowerUp;
    }

    bool hasPowerUp() {
      return _hasPowerUp;
    }
};
const int BRICK_WIDTH = 15;
const int BRICK_HEIGHT = 4;
const int BRICK_GAP = 1;
const int NUM_ROWS = 1;
const int NUM_BRICK_PER_ROW = 8;
Brick **_bricks;

// Ball
const int BALL_RADIUS = 2;
Ball _ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 2);
int _ballYSpeed = 1;
bool _ballActive = false;

// Power up
/**
 * @brief A PowerUp class that essentially is a rounded rectangle shape
 *
 */
class PowerUp : public Shape {
  protected:
    int _radius = 0;
    int _dropSpeed = 0;
    bool _active = false;

  public:
    PowerUp(int x, int y, int width, int height)
      : Shape(x, y, width, height)
    {
      // purposefully empty
    }

    void setRadius(int radius)  {
      _radius = radius;
    }

    void setDropSpeed(int dropSpeed) {
      _dropSpeed = dropSpeed;
    }

    void setActive(bool active) {
      _active = active;
    }

    bool isActive() {
      return _active;
    }

    void update() {
      if (_active) {
        _y += _dropSpeed;
      }
    }

    /**
     * @brief Draw the rounded rectangle
     *
     * @param display
     */
    void draw (Adafruit_SSD1306& display) override {
      // Draw rounded rectangle takes in (xTop, yTop, width, height, radius)
      // https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#rectangles-2002784-10
      if (_drawFill) {
        display.fillRoundRect(_x, _y, _width, _height, _radius, SSD1306_WHITE);
      } else{
        display.drawRoundRect(_x, _y, _width, _height, _radius, SSD1306_WHITE);
      }

      // We don't call the parent class draw() call because
      // the bounding box is simple another rectangle
      // Shape::draw(disp);
    }

    String getName() const override{
      return "PowerUp";
    }
};
PowerUp **_powerUps;
bool _hasPowerUp = false;

// FRS laser trigger
const int INPUT_FSR_PIN = A5;
const int GREEN_PIN = 21;
const int YELLOW_PIN = 17;
const int RED_PIN = 16;
class Laser : public Rectangle {
  protected:
    int _shootSpeed = 0;
    bool _active = false;

  public:
    Laser(int x, int y, int width, int height) : Rectangle(x, y, width, height)
    {
    }

    void setShootSpeed(int shootSpeed) {
      _shootSpeed = shootSpeed;
    }

    bool isActive() {
      return _active;
    }

    void setActive(bool active) {
      _active = active;
    }

    void update() {
      if (_active) {
        _y -= _shootSpeed;
      }
    }
};
Laser _laser(0, 0, 2, 6);
const char STR_LASER_READY[] = "Laser Ready!";

// Game state
enum GameState {
  NEW_GAME,
  PLAYING,
  GAME_OVER,
};
Preferences _preferences;  // For reading high score
const int DELAY_LOOP_MS = 5;
const int INPUT_DELAY_MS = 1000;
GameState _gameState = NEW_GAME;
int _gameModeIdx = 0;
int _score = 0;
int _highScore = 0;

void setup() {
  Serial.begin(9600);

  pinMode(LEFT_BTN_PIN, INPUT_PULLUP);
  pinMode(VIBRO_PIN, OUTPUT);
  pinMode(TONE_OUTPUT_PIN, OUTPUT);

  pinMode(INPUT_FSR_PIN, INPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);

  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(TONE_OUTPUT_PIN, PWM_CHANNEL);

  if (!lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }

  initializeOledAndShowLoadScreen();
  initializeGameEntities();
}

void loop() {
  _display.clearDisplay();
  _tone32.update();
  checkVibration();
  // Read analog joystick to control player ball
  _analogJoystick.read();

  int leftRightVal = _analogJoystick.getLeftRightVal();
  int btnVal = digitalRead(LEFT_BTN_PIN);
  if (_gameState == NEW_GAME) {
    int xMovementPixels = map(leftRightVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -2, 2);
    if (xMovementPixels > 0) {
      _gameModeIdx = max(0, _gameModeIdx - 1);
    } else if (xMovementPixels < 0) {
      _gameModeIdx = min(1, _gameModeIdx + 1);
    }
    showMenu();
    if (btnVal == LOW) {
      resetGameEntities();
      _gameState = PLAYING;
      delay(INPUT_DELAY_MS);
    }
  } else if (_gameState == PLAYING) {
    playLoop();
    drawStatusBar();
  } else if (_gameState == GAME_OVER) {
    scoreboard();
    _lastWin = millis();
    if (btnVal == LOW) {
      _gameState = NEW_GAME;
      delay(INPUT_DELAY_MS);
    }
  }
  _display.display();
  // Special handling for the case where the disappearance of last brick was not drawn before playing the sound
  if (_gameState == GAME_OVER && millis() - _lastWin > WIN_DELAY && !_playedWinGameSound) {
    playWinGameSound();
    _playedWinGameSound = true;
  }
  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}

/**
 * Displays the select game mode menu
 */
void showMenu() {
  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);

  int16_t x1, y1;
  uint16_t w, h;

  int yText = 0;
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

/**
 * Sets up the game components
 */
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
    int powerUpIdx = random(NUM_BRICK_PER_ROW);
    for (int j = 0; j < NUM_BRICK_PER_ROW; j++) {
      _bricks[i * NUM_BRICK_PER_ROW + j] = new Brick(brickX, brickY, BRICK_WIDTH, BRICK_HEIGHT);
      _bricks[i * NUM_BRICK_PER_ROW + j]->setDrawFill(true);
      brickX += BRICK_GAP + BRICK_WIDTH;
      // Assign powerups to one brick in each row
      if (j == powerUpIdx) {
        _bricks[i * NUM_BRICK_PER_ROW + j]->setHasPowerUp(true);
      }
    }
    brickX = 0;
    brickY += BRICK_GAP + BRICK_HEIGHT;
  }

  // powerup set up
  _powerUps = new PowerUp*[NUM_ROWS];
  for (int i = 0; i < NUM_ROWS; i++) {
    _powerUps[i] = new PowerUp(0, SCREEN_HEIGHT, 6, 6);
    _powerUps[i]->setDropSpeed(1);
    _powerUps[i]->setRadius(1);
    _powerUps[i]->setDrawFill(false);
  }

  // laser set up
  _laser.setShootSpeed(5);
}

/**
 * Resets all game component states upon new game
 */
void resetGameEntities() {
  resetBricks();
  resetPaddle();
  resetBall();
  resetPowerUps();
  _score = 0;
  _playedWinGameSound = false;
}

void resetBricks() {
  for (int i = 0; i < NUM_ROWS; i++) {
    int powerUpIdx = random(NUM_BRICK_PER_ROW);
    for (int j = 0; j < NUM_BRICK_PER_ROW; j++) {
      _bricks[i * NUM_BRICK_PER_ROW + j]->setHasPowerUp(false);
      _bricks[i * NUM_BRICK_PER_ROW + j]->setBroken(false);
      if (j == powerUpIdx) {
        _bricks[i * NUM_BRICK_PER_ROW + j]->setHasPowerUp(true);
      }
    }
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

void resetPowerUps() {
  for (int i = 0; i < NUM_ROWS; i++) {
    _powerUps[i]->setActive(false);
    _powerUps[i]->setLocation(0, SCREEN_HEIGHT);
  }
  _laser.setActive(false);
  _laser.setLocation(0, SCREEN_HEIGHT);
}

/**
 * Runs the main game loop
 */
void playLoop() {
  checkActivateBall();
  int pSpeed = updateAndGetPaddleSpeed();
  if (_gameModeIdx == 1) {  // Check for advanced mode updates
    checkLaser();
    updatePowerUps();
    updateLaser();
  }
  updateBall(pSpeed);
  if (!updateBricks()) {
    _gameState = GAME_OVER;
    // https://www.tutorialspoint.com/esp32_for_iot/esp32_for_iot_preferences.htm
    _preferences.begin("scoreRecord", false);
    if (_gameModeIdx == 0) {
      int oldHighscore = _preferences.getInt("basic", 0);
      _highScore = max(oldHighscore, _score);
      _preferences.putInt("basic", _highScore);
    } else {
      int oldHighscore = _preferences.getInt("advanced", 0);
      _highScore = max(oldHighscore, _score);
      _preferences.putInt("advanced", _highScore);
    }
    _preferences.end();
  }
}

/**
 * Checks for ball activation upon button press
 * When the ball is not activated, it will go along with the paddle
 * Upon activated, it will be assigned a random xSpeed 
 */
void checkActivateBall() {
  int btnVal = digitalRead(LEFT_BTN_PIN);
  if (!_ballActive && btnVal == LOW) {
    _ballActive = true;
    _ball.setSpeed(random(-2, 2), -_ballYSpeed);
  }
}

/**
 * Updates the paddle control based on the game mode
 * Basic (0) uses joystick input
 * Advanced (1) uses accelerometer input
 * Returns the padde speed for ball update later
 */
int updateAndGetPaddleSpeed() {
  int pSpeed = 0;
  if (_gameModeIdx == 0) {
    // Read joystick
    int leftRightVal = _analogJoystick.getLeftRightVal();
    int xMovementPixels = map(leftRightVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -5, 5);
    // Decide the actual paddle speed and location based on the joystick value
    pSpeed = -xMovementPixels;
  } else {
    // Read accelerometer
    sensors_event_t event;
    lis.getEvent(&event);
    pSpeed = -event.acceleration.x;
  }
  _paddle.setX(_paddle.getX() + pSpeed);
  _paddle.forceInside(0, 0, _display.width(), _display.height());
  _paddle.draw(_display);
  return pSpeed;
}

/**
 * Updates the ball movement incorporating paddle speed upon collision
 */
void updateBall(int pSpeed) {
  if (!_ballActive) {
    // If the ball is not active yet, make it stick with the paddle
    _ball.setCenter(_paddle.getX() + _paddle.getWidth() / 2, _paddle.getY() - _ball.getRadius() - 1);
    showServeInstruction();
  } else {
    // Check for collision with paddle and wall
    if (_ball.overlaps(_paddle) && _ball.getCenterY() <= _paddle.getTop() && _ball.getYSpeed() > 0) {
      // Ball overlaps with the paddle, bounce back up
      _ball.reverseYSpeed();
      _ball.setSpeed(constrain(_ball.getXSpeed() + pSpeed, -3, 3), _ball.getYSpeed());
    } else if (_ball.getTop() <= 8 && _ball.getYSpeed() < 0) {
      // Ball hits the ceiling, bounce back down
      _ball.reverseYSpeed();
    } else if (_ball.getTop() >= SCREEN_HEIGHT) {
      // Ball goes off screen from the bottom
      resetBall();
      _score -= 5;
      playLoseBallSound();
      startVibration();
    }
    // Ball hits the wall on left and right
    if (_ball.checkXBounce(0, SCREEN_WIDTH - 2 * _ball.getRadius())) {
      _ball.reverseXSpeed();
    }
    _ball.update();
  }
  _ball.draw(_display);
}

/**
 * Shows the powerup drop animation if any of them are active
 */
void updatePowerUps() {
  for (int i = 0; i < NUM_ROWS; i++) {
    _powerUps[i]->update();
    if (_powerUps[i]->isActive() && _powerUps[i]->overlaps(_paddle)) {
      // successfully picked up powerup
      _powerUps[i]->setActive(false);
      _hasPowerUp = true;
    } else if (_powerUps[i]->getTop() > SCREEN_HEIGHT) {
      // powerup goes off screen
      _powerUps[i]->setActive(false);
    }
    if (_powerUps[i]->isActive()) {
      _powerUps[i]->draw(_display);
    }
  }
}

/**
 * Incorporates laser charge LED display and laser shooting based on FRS input
 */
void checkLaser() {
  if(_hasPowerUp) {
    int val = analogRead(INPUT_FSR_PIN);
    int index = map(val, 2000, 4095, 0, 5);
    // Laser charge sequence
    if (index >= 1) {
      digitalWrite(GREEN_PIN, HIGH);
      _tone32.playTone(300);
    }
    if (index >= 2) {
      digitalWrite(YELLOW_PIN, HIGH);
      _tone32.playTone(400);
    }
    if (index >= 3) {
      digitalWrite(RED_PIN, HIGH);
      _tone32.playTone(500);
      
    }
    if (index >= 4) {
      shootLaser();
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(RED_PIN, LOW);
      _hasPowerUp = false;
    }

    if (index == 0) {
      _tone32.playTone(0);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(RED_PIN, LOW);
    }
  }
}

void shootLaser() {
  _laser.setLocation((_paddle.getLeft() + _paddle.getRight()) / 2, (_paddle.getTop() + _paddle.getBottom()) / 2);
  _laser.setActive(true);
}

void updateLaser() {
  _laser.update();
  if (_laser.isActive()) {
    _laser.draw(_display);
  }
}

/**
 * Updates the state of the bricks
 * Returns true if all bricks are broken
 */
bool updateBricks() {
  bool hasUnbrokenBrick = false;
  for (int i = 0; i < NUM_ROWS * NUM_BRICK_PER_ROW; i++) {
    Brick *brick = _bricks[i];
    if (brick->overlaps(_ball) && !brick->isBroken()) {
      brick->setBroken(true);
      _score += 1;
      playBreakBrickSound();
      if (_ball.getCenterY() >= brick->getBottom() && _ball.getYSpeed() < 0) {
        // Ball hits the brick from the bottom
        _ball.reverseYSpeed();
      } else if (_ball.getTop() < brick->getBottom()) {
        // Ball hits the brick from the side
        _ball.reverseXSpeed();
      }
      if (brick->hasPowerUp()) {
        // Activates the corresponding powerup
        _powerUps[i / NUM_BRICK_PER_ROW]->setLocation((brick->getLeft() + brick->getRight()) / 2, (brick->getTop() + brick->getBottom()) / 2);
        _powerUps[i / NUM_BRICK_PER_ROW]->setActive(true);
      }
    }

    if (brick->overlaps(_laser) && !brick->isBroken()) {
      brick->setBroken(true);
      _score += 1;
      playBreakBrickSound();
      _laser.setActive(false);
      if (brick->hasPowerUp()) {
        _powerUps[i / NUM_BRICK_PER_ROW]->setLocation((brick->getLeft() + brick->getRight()) / 2, (brick->getTop() + brick->getBottom()) / 2);
        _powerUps[i / NUM_BRICK_PER_ROW]->setActive(true);
      }
    }

    if (!brick->isBroken()) {
      brick->draw(_display);
      hasUnbrokenBrick = true;
    }
  }
  return hasUnbrokenBrick;
}

/**
 * Shows press A to serve on screen while ball is not activated
 */
void showServeInstruction() {
  int16_t x1, y1;
  uint16_t w, h;
  int yText = 24;
  _display.setTextSize(1);
  _display.getTextBounds(STR_SERVE, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_SERVE);
}

/**
 * Plays the brick-breaking sound and start the timer
 */
void playBreakBrickSound() {
  _tone32.playTone(COLLISION_TONE_FREQUENCY, PLAY_TONE_DURATION_MS);
}

/**
 * Plays the ball-losing sound and start the timer
 */
void playLoseBallSound() {
  _tone32.playTone(LOSE_BALL_TONE_FREQUENCY, PLAY_TONE_DURATION_MS);
}

/**
 * Plays the sound effect when the player clears all bricks
 */
void playWinGameSound() {
  _tone32.playTone(KEY_C, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(KEY_E, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(KEY_G, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(KEY_E, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(KEY_G, PLAY_TONE_DURATION_MS);
  delay(PLAY_TONE_DURATION_MS);
  _tone32.playTone(0);
}

/**
 * Activate the lose ball vibration
 */
void startVibration() {
  _vibrationStart = millis();
  digitalWrite(VIBRO_PIN, HIGH);
}

/**
 * Check and see if the vibration has exceeded the predefined interval, if so turn it off
 */
void checkVibration() {
  if (millis() - _vibrationStart > VIBRATION_INTERVAL) {
    digitalWrite(VIBRO_PIN, LOW);
  }
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
 * Initializes the OLED screen
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

/**
 * Displays the status bar, which only shows the current score
 */
void drawStatusBar() {
  int16_t x1, y1;
  uint16_t w, h;
  _display.setTextColor(WHITE, BLACK);
  // Draw accumulated points
  _display.setTextSize(1);
  _display.setCursor(0, 0); // draw points
  _display.print(_score);
  if (_hasPowerUp) {
    _display.getTextBounds(STR_LASER_READY, 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() - w, 0); // draw laser ready text
    _display.print(STR_LASER_READY);
  }
}
