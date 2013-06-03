/******************************************************************                                                LED Driver implementation for Oasis TOFD-5465GGH-B 4-Digit LED                                                    displays.  The display is based upon the PTC PT6961 LED driver IC.                                                Since the driver was built based upon the IC datasheet, it should                                                 be applicable to any other displays based upon the PT6961.                                                        
 Written by Steve Davidson (buzzdavidson)                                                                            
 Licensed under terms of BSD license; check license.txt for more
 information.                                                                                                     

 All above text must be included in any redistribution.                                                           *****************************************************************/

#include <stdint.h>
#include <avr/pgmspace.h>

#ifndef OASISLED_H
#define OASISLED_H

/** Font data: Numeric chars '0' through 'F' */
static uint8_t OASIS_FONT_DATA[] PROGMEM  = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x58, 0x5e, 0x79, 0x71 };

/** Spinner data for SPIN_FULL implementation.  This could be generated programmatically, but this is a simple first-pass implementation. */
static uint8_t OASIS_FULL_SPIN_DATA[] PROGMEM = {  
  0x00, 0x00, 0x00, 0x01, 
  0x00, 0x00, 0x00, 0x02, 
  0x00, 0x00, 0x00, 0x04, 
  0x00, 0x00, 0x00, 0x08,
  0x00, 0x00, 0x08, 0x00,
  0x00, 0x08, 0x00, 0x00,
  0x08, 0x00, 0x00, 0x00,
  0x10, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x00
};

/** Spinner type enumeration */
typedef enum OasisSpinType { SPIN_NONE, SPIN_DIGIT, SPIN_FULL };
  
/**
  Specifically written to support Oasis TQFD-5465GGH-B  4-Digit LED displays,
  This class will support any PT6961 based displays; since the Oasis display
  Does not support keyboard integration without performing surgery, this 
  Implementation does not currently implement that behavior.  Extension to 
  Support keyscan and debounce should be trivial.
*/
class OasisLED {

private:
  uint8_t _clock;          /**< Clock pin */
  uint8_t _data;           /**< Data pin */
  uint8_t _strobe;         /**< Strobe pin */
  bool _colonState;        /**< Colon mode (true=enabled) */
  uint8_t _digits[4];      /**< Stored digit values */
  uint8_t _digitCount;     /**< Number of digits */
  uint8_t _layout;         /**< Display layout */
  bool _displayState;      /**< Display state (enabled?) */
  uint8_t _brightness;     /**< Display brightness (3 bit value) */
  OasisSpinType _spinMode; /**< Spin mode */
  uint8_t _spinPos;        /**< Mode-specific counter for spinner implementation */
  
  /** Default display consists of four digits */
  const static uint8_t OASIS_DEFAULT_DIGIT_COUNT = 4;
  /** Number of discrete steps for SPIN_DIGIT spinner implementation */
  const static uint8_t OASIS_SPIN_DIGIT_STEPS = 6;
  /** Number of discrete steps for SPIN_FULL implementation (4 digit version) */
  const static uint8_t OASIS_SPIN_DISPLAY_STEPS = 12;
  /** Value added to digits 0 and 1 to enable colon.  */
  const static uint8_t OASIS_COLON_BITS = 0x80;
  
  /** Command: display layout */
  const static uint8_t OASIS_CMD_DISPLAY_LAYOUT = 0x00;
  /** Command: data mode */
  const static uint8_t OASIS_CMD_DATA_SET = 0x40;
  /** Data mode: Write (display output) */
  const static uint8_t OASIS_BIT_DATA_WRITE = 0x00;
  /** Data mode: Read (keyscan mode - not available with this display */
  const static uint8_t OASIS_BIT_DATA_READ = 0x02;
  /** Memory addressing mode: Auto increment address after update */
  const static uint8_t OASIS_BIT_ADR_INCREMENT = 0x00;
  /** Memory addressing mode: Do not adjust address afer update */
  const static uint8_t OASIS_BIT_ADR_FIXED = 0x04;
  /** Operation mode: normal operation */
  const static uint8_t OASIS_BIT_MODE_NORMAL = 0x00;
  /** Operation mode: test mode */
  const static uint8_t OASIS_BIT_MODE_TEST = 0x08;
  /** Command: Set memory address - Digit 0 starts at address 0, each digit 2 bytes */
  const static uint8_t OASIS_CMD_MEM_ADDR = 0xC0;
  /** Brightness mask: this is a 3 bit value */
  const static uint8_t OASIS_MASK_BRIGHTNESS = 0x07;
  /** Command: Set display modes (lower 3 bits set brightness (no constants available for these) */
  const static uint8_t OASIS_CMD_DISP_CTRL = 0x80;
  /** Display control: Turn display off */
  const static uint8_t OASIS_BIT_DISP_OFF = 0x00;
  /** Display control: Turn display on */
  const static uint8_t OASIS_BIT_DISP_ON = 0x08;
  
  /** (Private method) Update display state - also provides brightness control. */
  void _updateDisplayState(bool state);
  /** (Private method) Wrapper method to send single discrete command to display */ 
  void _sendCommand(uint8_t cmd);
  
public:
  /** Layout Mode: 6 12-segment digits */
  const static uint8_t OASIS_BIT_LAYOUT_6x12 = 0x02;
  /** Layout Mode: 7 11-segment digits */
  const static uint8_t OASIS_BIT_LAYOUT_7x11 = 0x03;
  /** Default brightness value.  Brightness is a 3-bit value (0 to 7) */
  const static uint8_t OASIS_DEFAULT_BRIGHTNESS = 3;

  /** Object constructor
     \param clockPin pin to use for clock
     \param strobePin pin to use for data strobe
     \param dataPin pin to use for data in 
     \param digitCount (optional) - specify number of digits on display (default 4)
     \param displayLayout (optional) - select layout (OASIS_BIT_LAYOUT_6x12 [default] or OASIS_BIT_LAYOUT_7x11)
  */
  OasisLED(uint8_t clockPin, uint8_t strobePin, uint8_t dataPin, uint8_t digitCount = OASIS_DEFAULT_DIGIT_COUNT, uint8_t displayLayout = OASIS_BIT_LAYOUT_6x12);

  /** Turn display off */
  void disableDisplay();  

  /** Turn display on (Default state at startup - only used after calling disableDisplay) */
  void enableDisplay();

  /** Retrieve bitmap for specified character.
    \param value Character to retrieve
    \return Bitmap for character suitable for sending to setDigitElements method.  If character is undefined, will return 0.
  */
  uint8_t getDisplayBits(char value);

  /** Redraw the display */
  void redraw();

  /** Reset display to power-on state */
  void reset();
  
  /** Initialize Display */
  void initialize();

  /** Set brightness level.  
     \param bright This is a 3-bit value, with 0 being most dim and 7 being brightest. Only 3 lowest bits are utilized. 
  */
  void setBrightness(uint8_t bright);

  /** Control colon display
     \param state Colon display state: true = on
  */
  void setColon(bool state);

  /** Set specified digit to provided value.
     \param digit Digit to update (Leftmost digit is 0)
     \param value Character to display (0x00 through 0x0F)
     \param redraw (optional; default=true) If true, display will be updated immediately.  If false, user must call redraw() to update display. 
  */
  void setDigit(uint8_t digit, uint8_t value, bool redraw = true);

  /** Control individual segments of display.  Segment 0 is topmost segment.  
      Segment numbering increases in clockwise fashion, with center segment being number 6.  
      Colon is implemented as bit 7 of digits 1 and 2; this could be used to control individual 
      dots of colon as alternative to setColon method - to use this functionality, you would need
      to call setColon(false) first.
      \param digit Digit to update (Leftmost digit is 0)
      \param elements Bitmap for element control.  See method getCharacterBits(char) for details.
  */
  void setDigitSegments(uint8_t digit, uint8_t elements);

  /** Set spinner mode.  This effect rotates a segment around the leftmost digit (SPIN_DIGIT) or around the entire display (SPIN_FULL).
      Note: SPIN_FULL is currently hard-coded to use 4 digit positions.  Implementation will need to be modified if this library is 
      expanded to use different display with more or less digits.  SPIN_DIGIT is currently device-independent.
      Used in conjunction with spin() method.
      \param spin Spin mode: one of SPIN_NONE (n/a), SPIN_DIGIT (spin single digit position), SPIN_FULL (spin around entire display)
      \sa spin
  */
  void setSpinnerMode(OasisSpinType spin);
  
  /** Advance the configured spinner by one step.
      \sa setSpinnerMode 
  */
  void spin();
  
  /** Directly set numeric value of display - allows values 0 to 9999 for the default display.
      \param value Numeric value to set
      \param showLeadingZeroes if true, value will be padded with zeroes
  */
  void setValue(uint16_t value, bool showLeadingZeroes=true);
  
  /**
      Helper method to toggle colon display
  */
  void toggleColon();

};

#endif
