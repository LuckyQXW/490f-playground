#include <Preferences.h>

Preferences _preferences;

void setup() {
  // put your setup code here, to run once:
  _preferences.begin("scoreRecord", false);
  int oldHighscore = _preferences.getInt("advanced", 0);
  _preferences.putInt("advanced", 0);
  oldHighscore = _preferences.getInt("basic", 0);
  _preferences.putInt("basic", 0);
  _preferences.end();
}

void loop() {
  // put your main code here, to run repeatedly:

}
