# Spin-FV-1-based-programmable-multi-effect

Based on the FV-1 datasheet, Hagiwo design and various versions of the FV-1 effects found on the internet, this is my own take on a programmable multi effects unit.

The programmer is based around a Teensy 2++, I understand these are hard to find now, but a Teensy 4.1 or similar can be used in its place, just note that if you do use a 3.3v compatible board then you can probably remove the level converters and just bypass them. Also if your board has an onboard SD card slot then again you can use that instead of the external one I have used. Basically I was just using up old Arduino parts I had laying around.

# Functions

32 effects cores based on the 8 internal effects of the chip and 24 external effects on 3 eeprom chips.
Each effect has upto 3 variations and a wet/dry mix.
All parameters are stored for each effect.
999 patches can be stored on the SD card.
MIDI input to recall patches and also edit effects in realtime with CC numbers.
Mono/Stereo inputs and outputs.

