/****************************************************************** 
 LED Driver implementation for Oasis TOFD-5465GGH-B 4-Digit LED 
 displays.  The display is based upon the PTC PT6961 LED driver IC. 
 Since the driver was built based upon the IC datasheet, it should
 be applicable to any other displays based upon the PT6961.

 Written by Steve Davidson (buzzdavidson)

 Licensed under terms of BSD license; check license.txt for more
 information.

 All above text must be included in any redistribution.                                                            *****************************************************************/

#include "OasisLED.h"
#include <Arduino.h>
#include <avr/pgmspace.h>

OasisLED::OasisLED(uint8_t clockPin, uint8_t strobePin, uint8_t dataPin, uint8_t digitCount, uint8_t displayLayout) {
  _clock = clockPin;
  _data = dataPin;
  _strobe = strobePin;
  pinMode(_clock, OUTPUT);
  pinMode(_data, OUTPUT);
  pinMode(_strobe, OUTPUT);
  _layout = displayLayout;
  _digitCount = digitCount;
}

void OasisLED::disableDisplay() {
  _updateDisplayState(false);
}

void OasisLED::enableDisplay() {
  _updateDisplayState(true);
}

uint8_t OasisLED::getDisplayBits(char value) {
  return pgm_read_byte(OASIS_FONT_DATA + value);
}

void OasisLED::initialize() {
  reset();
}

void OasisLED::redraw() {
  digitalWrite(_strobe, LOW);
  for (uint8_t i = 0; i < _digitCount; i++) {
    shiftOut(_data, _clock, LSBFIRST, OASIS_CMD_MEM_ADDR + (2 * i));
    if (_colonState && i < 2) {
      shiftOut(_data, _clock, LSBFIRST, _digits[i] | OASIS_COLON_BITS);
    } else {
      shiftOut(_data, _clock, LSBFIRST, _digits[i]);
    }
  } 
  digitalWrite(_strobe, HIGH);
}

void OasisLED::reset() {
  _displayState = true;
  _colonState = false;
  _brightness = OASIS_DEFAULT_BRIGHTNESS;
  _spinMode = SPIN_NONE;
  _spinPos = 0;
  memset(_digits, 0, _digitCount);
  _sendCommand(OASIS_CMD_DISPLAY_LAYOUT | _layout);
  _sendCommand(OASIS_CMD_DATA_SET | OASIS_BIT_DATA_WRITE | OASIS_BIT_ADR_INCREMENT | OASIS_BIT_MODE_NORMAL);
  _updateDisplayState(_displayState);
  redraw();
}

void OasisLED::setBrightness(uint8_t bright) {
  // Brightness is a 3-bit value (0 to 7)
  _brightness = (bright & OASIS_MASK_BRIGHTNESS);
  _updateDisplayState(_displayState);
}

void OasisLED::setColon(bool state) {
  if (state != _colonState) {
    _colonState = state;
    redraw();
  }
}

void OasisLED::setDigitSegments(uint8_t digit, uint8_t elements) {
  if (digit < _digitCount) {
    _digits[digit] =  elements; 
  }
}

void OasisLED::_sendCommand(uint8_t cmd) {
  digitalWrite(_strobe, LOW);
  shiftOut(_data, _clock, LSBFIRST, cmd);
  digitalWrite(_strobe, HIGH);
}

void OasisLED::setDigit(uint8_t digit, uint8_t value, bool redraw) {
  if (digit < _digitCount) {
    _digits[digit] =  getDisplayBits(value & 0x0F); 
  }
  if (redraw) {
    this->redraw();
  }
}

void OasisLED::setSpinnerMode(OasisSpinType spin) {
  if (spin != _spinMode) {
    _spinMode = spin;
  }
  _spinPos = 0;
}

uint16_t _int10pow(uint16_t pow) {
  // built-in pow() method drags in floating point support and bloats library
  uint16_t rv = 1;
  uint8_t n = 1;
  if (pow >= 0 && pow < 8) {
    while (n++ < pow) {
      rv *= 10;
    }
  }
  return rv;
}

void OasisLED::setValue(uint16_t value, bool showLeadingZeroes) {
  uint16_t v = 0;
  uint8_t digit = 0;
  uint16_t curValue = 0;
  for (uint8_t i = 0; i < _digitCount; i++) {
    v = (value % _int10pow(i + 2)) / _int10pow(i + 1);
    curValue += v * _int10pow(i + 1);
    if (v > 0 || (v == 0 && (showLeadingZeroes || curValue < value))) {
      setDigit(_digitCount - digit - 1, v, false);
    } else {
      setDigitSegments(_digitCount - digit - 1, 0x00);
    }
    digit++; 
  }
  redraw();
}

void OasisLED::spin() {
  if (_spinMode != SPIN_NONE) {
    _spinPos++;
    if (_spinPos >= (_spinMode == SPIN_DIGIT ? OASIS_SPIN_DIGIT_STEPS : OASIS_SPIN_DISPLAY_STEPS)) {
      _spinPos = 0;
    }
    switch (_spinMode) {
      case SPIN_DIGIT:      
        setDigitSegments(0, 1 << _spinPos);
        break;
      case SPIN_FULL:
        for (int i = 0; i < 4; i++) {
          setDigitSegments(i, pgm_read_byte(OASIS_FULL_SPIN_DATA + (_spinPos * 4) + i));
        }
        break;
    }
    redraw();
  }
}

void OasisLED::toggleColon() {
  setColon(!_colonState);
}

void OasisLED::_updateDisplayState(bool state) {
  _displayState = state;
  _sendCommand(OASIS_CMD_DISP_CTRL | (_displayState ? OASIS_BIT_DISP_ON : OASIS_BIT_DISP_OFF) | _brightness);
}


