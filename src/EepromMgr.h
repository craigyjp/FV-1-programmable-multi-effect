#include <EEPROM.h>

#define EEPROM_MIDI_CH 0
#define EEPROM_ENCODER_DIR 1
#define EEPROM_LED_INTENSITY 2
#define EEPROM_SEND_PARAMS 3

int getMIDIChannel() {
  byte midiChannel = EEPROM.read(EEPROM_MIDI_CH);
  if (midiChannel < 0 || midiChannel > 16) midiChannel = MIDI_CHANNEL_OMNI;//If EEPROM has no MIDI channel stored
  return midiChannel;
}

void storeMidiChannel(byte channel)
{
  EEPROM.update(EEPROM_MIDI_CH, channel);
}

boolean getEncoderDir() {
  byte ed = EEPROM.read(EEPROM_ENCODER_DIR); 
  if (ed < 0 || ed > 1)return true; //If EEPROM has no encoder direction stored
  return ed == 1 ? true : false;
}

void storeEncoderDir(byte encoderDir)
{
  EEPROM.update(EEPROM_ENCODER_DIR, encoderDir);
}

boolean getSendParams() {
  byte sp = EEPROM.read(EEPROM_SEND_PARAMS); 
  if (sp < 0 || sp > 1)return true; //If EEPROM has no encoder direction stored
  return sp == 1 ? true : false;
}

void storeSendParams(byte updateParams)
{
  EEPROM.update(EEPROM_SEND_PARAMS, updateParams);
}

int getLEDintensity() {
  byte li = EEPROM.read(EEPROM_LED_INTENSITY);
  if (li < 0 || li > 10) li = 10; //If EEPROM has no intesity stored
  return li;
}

void storeLEDintensity(byte LEDintensity){
  EEPROM.update(EEPROM_LED_INTENSITY, LEDintensity);
}

