# Spin-FV-1-based-programmable-multi-effect

Based on the FV-1 datasheet, Hagiwo design and various versions of the FV-1 effects found on the internet, this is my own take on a programmable multi effects unit.

![Synth](photos/synth.jpg)

The programmer is based around a Teensy 2++, I understand these are hard to find now, but a Teensy 4.1 or similar can be used in its place, just note that if you do use a 3.3v compatible board then you can probably remove the level converters and just bypass them. Also if your board has an onboard SD card slot then again you can use that instead of the external one I have used. The board must support eeprom of some kind to store parameters such as MIDI channel permanently so either onboard eeprom, a local i2c chip etc. For the DAC I used the DAC8568-A variant, it produces 0-2.5v output and I have level converters for the 3.3v requirements. If you use a DAC8568-D variation it will output 0-5V and you can basically swap out the level converters for voltage followers same as the dry/wet mix CV's. But you will also have to scale the DAC voltages again to producs 0-3.3v and 0-2v as required. The A variant can be obtained for around $5, whereas the D version can be around $25. Basically I was just using up old Arduino parts I had laying around. Patches are limited to 128 with a Teensy 2++, it simply will not boot any higher.

# Functions

* 32 effects cores based on the 8 internal effects of the chip and 24 external effects on 3 eeprom chips.
* Each effect has upto 3 variations and a wet/dry mix.
* All parameters are stored for each effect.
* Parameters can be sent over MIDI or disabled.
* 128 patches can be stored on the SD card.
* MIDI in to recall patches and also edit effects in realtime with CC numbers.
* MIDI out for parameter sending and future sysex dumps
* MIDI thru.
* Mono/Stereo inputs and outputs.
* Input monitoring VU's
* Footswitch changes pot3 and can affect the speed of rotary sounds from fast to slow with a slope.

# Eeproms

I have included 3 eeproms that I have used in my build, and others that I found. What is quite annoying is that you cannot read an eeprom back into the SpinCad Designer, although it has an import function I never managed to get it to do anything except blank the patch I loaded the eeprom into. So I found this utility on github that allows you to read in an eeprom and spit it out in the Arduino IDE serial console as 8 progmem dumps. These can then be used to create an effects file of 8 progmem sections that can be written to a blank eeprom. This way it's possible to test and listen to eeproms from others, cut out the effect you want and build you own banks of 8.

# Todo

* Sysex dumps and loading of patches
