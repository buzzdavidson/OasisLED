#include "OasisLED.h"
#include <Arduino.h>

uint8_t clockPin = 3;
uint8_t strobePin = 2;
uint8_t dataPin = 4;

OasisLED ledDisplay = OasisLED(clockPin, strobePin, dataPin);

void setup() {
  ledDisplay.initialize();
}

void loop() {
  // Show "Spinning" display across all digits
  ledDisplay.setSpinnerMode(SPIN_FULL);
  for (int i = 0; i < 24; i++) {
    ledDisplay.spin();
    delay(100);
  }
  
  // Show "spinning" display on first digit while counting on last digit
  ledDisplay.setSpinnerMode(SPIN_DIGIT);
  for (int c = 1; c <= 3; c++) {
    for (int i = 0; i < 16; i++) {
      ledDisplay.setDigit(3, i, true);
      ledDisplay.spin();
      delay(100);
    }
  }
  
  ledDisplay.setSpinnerMode(SPIN_NONE);
  
  // Count up from 0 to 99 (leading zeroes enabled)
  for (int i = 0; i < 30; i++) {
    ledDisplay.setValue(i);
    delay(100);
  }
  
  // Clear display
  ledDisplay.setValue(0, false);
  
  // Count down from 99 to 0 (leading zeroes disabled)
  for (int i = 30; i >= 0; i--) {
    ledDisplay.setValue(i, false);
    delay(100);
  }
  
  // Test brightness: show each level for 1 second
  ledDisplay.setDigit(0, 0x0b);
  for (int i = 0; i <=7; i++) {
    ledDisplay.setDigit(3, i);
    ledDisplay.setBrightness(i);
    delay(1000);
  }
  ledDisplay.reset();
  
  // Show test cases
  ledDisplay.setValue(1234);
  delay(1000);
  // Leading zeroes true, output should be "0876"
  ledDisplay.setValue(876, true);
  delay(1000);
  // Leading zeroes false, output should be "25"
  ledDisplay.setValue(25, false);
  delay(1000);
  // Leading zeroes true, output should be "0000"
  ledDisplay.setValue(0, true);  
  delay(1000);
  // Leading zeroes false, output should be blank
  ledDisplay.setValue(0, false);  
  delay(1000);
  // Make sure digit blanking doesn't interfere with intermediate zeroes
  // Output should be "9099"
  ledDisplay.setValue(9099, false);
  delay(1000);
  // Output should be "9099"
  ledDisplay.setValue(9099, true);
  delay(1000);
  // Output should be "1001"
  ledDisplay.setValue(1001, false);
  delay(1000);
  
  // Countdown timer: 10 seconds with toggling colon  
  for (int i = 30; i >= 0; i--) {
    ledDisplay.toggleColon();
    ledDisplay.setValue(i);
    delay(500);
    ledDisplay.toggleColon();
    delay(500);
  }
  
  ledDisplay.reset();
  delay(2000);
}
