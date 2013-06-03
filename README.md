OasisLED
========

Arduino library for PT6961-based Oasis TOFD-5465GGH-B 4 digit 7-segment displays 

This is a reasonably feature-complete driver for Oasis TOFD-546x LED Displays.  It should be applicable to any LED displays based upon the PT6961 driver IC from Princeton Technology Corp.  The driver supports all functionality documented in the PT6961 datasheet dated January 2010 with the exception of keyscan support.  As the Oasis display is potted in epoxy and does not expose all of the IC pins without surgical intervention, this functionality has been left as an excercise for the more adventurous user.

I've tried to avoid restricting the implementation to Oasis LED display constraints.  While driver defaults support the displays with minimal fuss, it also provides facilities to support other PT6961 implementations.  I'd love to hear about issues encountered with other implementations as well as successes.

This display seems to be quite popular on eBay as of this date - I purchased 10 displays in a "buy it now" auction for $10 with free shipping (from a U.S seller no less).  The raw PT6961 driver IC seems to be unobtanium without resorting to mass purchase or dodgy eBay auctions.

Datasheet for the PT6961 is available [here](http://www.alldatasheet.com/datasheet-pdf/pdf/391689/PTC/PT6961.html). 

Device Details
--------------

When viewed from the top, the pinout is as follows:

1. Vss (gnd)
2. Data Out (not used by this driver)
3. Data In
4. Clock
5. Strobe
6. Vdd (+5vdc)

Individual segments are addressed in typical 7-segment fashion, with bit 0 (lsb) at the 12 o'clock position proceeding clockwise followed by the center segment at bit 6.  Bit 7 controls the colon display, with digit 0 (leftmost) controlling the top dot and digit 1 controlling the bottom dot.  These details are managed by the driver, but may be useful if independent colon dot display is desired.

Driver Features
---------------

The following features are supported by this driver:

* Simple numeric value setting with leading-zero suppression support (setValue)
* Single digit update capability with hex support (setDigit)
* Direct access to individual digit segments (setDigitSegments)
* Colon indicator control (setColon / toggleColon)
* 8-Step brightness control (setBrightness)
* 3 distinct "spinner" modes for activity display

Peculiarities of Oasis Display
------------------------------

The Oasis TOFD-5465GGH-B does have some peculiarities, namely:

* Decimal point is not accessible
* Main display is green, while colon seperator is red
* The PT6961 supports 8 levels of dimming (PWM controlled via 3-bit value), but only the first four seem distinct

