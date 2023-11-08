// Constants
#define RE_READ -9
#define NO_OF_PARAMS 15
const char* INITPATCHNAME = "Initial Patch";
#define HOLD_DURATION 1500
const uint32_t CLICK_DURATION = 250;
#define PATCHES_LIMIT 128
const String INITPATCH = "Solina,1,1,1,1,1,1,1,1,1,1,1";
const char* VERSION = "1.0";

#define   CCpot1 9
#define   CCpot2 10
#define   CCpot3 11
#define   CCbank0 12
#define   CCbank1 13
#define   CCbank2 14
#define   CCbank3 15
#define   CCeffect 16
#define   CCmix 17

// Parameters

String patchNumber;
unsigned long timeout = 0;
unsigned long firsttimer = 0;

byte LEDintensity = 10;//(EEPROM)
byte oldLEDintensity;

String patchName = INITPATCHNAME;
boolean encCW = true;//This is to set the encoder to increment when turned CW - Settings Option

//Values below are just for initialising and will be changed when synth is initialised to current panel controls & EEPROM settings
byte midiChannel = MIDI_CHANNEL_OMNI;//(EEPROM)
boolean announce = true;
boolean updateParams = false;

int currentbank = 0;
int pot1 = 0;
int pot1str = 0;
int pot2 = 0;
int pot2str = 0;
int pot3 = 0;
int pot3str = 0;
int stage = 0;
int stagestr = 0;
int effect = 0;
int effectstr = 0;
int mix = 0;
int mixstra = 0;
int mixstrb = 0;
boolean bank0 = 0;
boolean bank1 = 0;
boolean bank2 = 0;
boolean bank3 = 0;
int i = 0;

int returnvalue = 0;

uint32_t int_ref_on_flexible_mode = 0b00001001000010100000000000000000;  // { 0000 , 1001 , 0000 , 1010000000000000 , 0000 }

uint32_t sample_data = 0b00000000000000000000000000000000;

uint32_t channel_a = 0b00000010000000000000000000000000;
uint32_t channel_b = 0b00000010000100000000000000000000;
uint32_t channel_c = 0b00000010001000000000000000000000;
uint32_t channel_d = 0b00000010001100000000000000000000;
uint32_t channel_e = 0b00000010010000000000000000000000;
uint32_t channel_f = 0b00000010010100000000000000000000;
uint32_t channel_g = 0b00000010011000000000000000000000;
uint32_t channel_h = 0b00000010011100000000000000000000;

//OLED display data table
//PRESET 1st line
const char str01[8][12] PROGMEM = {//str1 is effect name of 1st line
 "Chorus", "Flange", "Tremolo", "Pitch", "Pitch", "No Effect", "Reverb", "Reverb"
};
const char* const name01[] PROGMEM = {
 str01[0], str01[1], str01[2], str01[3], str01[4], str01[5], str01[6], str01[7],
};

//PRESET 2nd line
const char str02[8][12] PROGMEM = {//str1 is effect name of 1st line
 "Reverb", "Reverb", "Reverb", "Shift", "Echo", " ", "1", "2"
};
const char* const name02[] PROGMEM = {
 str02[0], str02[1], str02[2], str02[3], str02[4], str02[5], str02[6], str02[7],
};

//PRESET param1
const char str03[8][12] PROGMEM = {//str1 is effect name of 1st line
 "rev mix", "rev mix", "rev mix", "pitch", "pitch", "-", "rev time", "rev time"
};
const char* const name03[] PROGMEM = {
 str03[0], str03[1], str03[2], str03[3], str03[4], str03[5], str03[6], str03[7],
};

//PRESET param2
const char str04[8][12] PROGMEM = {//str1 is effect name of 1st line
 "cho rate", "flng rate", "trem rate", "-", "echo delay", "-", "HF filter", "HF filter"
};
const char* const name04[] PROGMEM = {
 str04[0], str04[1], str04[2], str04[3], str04[4], str04[5], str04[6], str04[7],
};

//PRESET param3
const char str05[8][12] PROGMEM = {//str1 is effect name of 1st line
 "cho mix", "flng mix", "trem mix", "-", "echo mix", "-", "LF filter", "LF filter"
};
const char* const name05[] PROGMEM = {
 str05[0], str05[1], str05[2], str05[3], str05[4], str05[5], str05[6], str05[7],
};

//ROM1 1st line
const char str11[8][12] PROGMEM = {//str1 is effect name of 1st line
"Qubit","SuperNova","Modulated","Echo","Shimmer","Sample &","Modulated","Glitch Bit"
};
const char* const name11[] PROGMEM = {
 str11[0], str11[1], str11[2], str11[3], str11[4], str11[5], str11[6], str11[7],
};

//ROM1 2nd line
const char str12[8][12] PROGMEM = {//str1 is effect name of 1st line
"Delay","Delay","Delay","Reverb","Reverb","Hold","Reverb","Delay"
};
const char* const name12[] PROGMEM = {
 str12[0], str12[1], str12[2], str12[3], str12[4], str12[5], str12[6], str12[7],
};

//ROM1 param1
const char str13[8][12] PROGMEM = {//str1 is effect name of 1st line
"time","time","time","rev level","dwell","time","dwell","delay 1"

};
const char* const name13[] PROGMEM = {
 str13[0], str13[1], str13[2], str13[3], str13[4], str13[5], str13[6], str13[7],
};

//ROM1 param2
const char str14[8][12] PROGMEM = {//str1 is effect name of 1st line
"modulation","filter","feedback","delay time","pitch","feedback","depth","delay 2"

};
const char* const name14[] PROGMEM = {
 str14[0], str14[1], str14[2], str14[3], str14[4], str14[5], str14[6], str14[7],
};

//ROM1 param3
const char str15[8][12] PROGMEM = {//str1 is effect name of 1st line
"feedback","feedback","modulation","echo level","blend","modulation","rate","feedback"

};
const char* const name15[] PROGMEM = {
 str15[0], str15[1], str15[2], str15[3], str15[4], str15[5], str15[6], str15[7],
};

//ROM2 1st line
const char str21[8][12] PROGMEM = {//str1 is effect name of 1st line
"Daydream","Starfield","Dual Pitch","Triple","Reverse","Wah","Vibrato","Phaser"
};
const char* const name21[] PROGMEM = {
 str21[0], str21[1], str21[2], str21[3], str21[4], str21[5], str21[6], str21[7],
};

//ROM2 2nd line
const char str22[8][12] PROGMEM = {//str1 is effect name of 1st line
"Delay","Delay","Shift","Delay","Delay","Reverb","Reverb","Reverb"
};
const char* const name22[] PROGMEM = {
 str22[0], str22[1], str22[2], str22[3], str22[4], str22[5], str22[6], str22[7],
};

//ROM2 param1
const char str23[8][12] PROGMEM = {//str1 is effect name of 1st line
"time","time","pitch 1","delay 1","sample","reverb","reverb","reverb"
};
const char* const name23[] PROGMEM = {
 str23[0], str23[1], str23[2], str23[3], str23[4], str23[5], str23[6], str23[7],
};

//ROM2 param2
const char str24[8][12] PROGMEM = {//str1 is effect name of 1st line
"feedback","feedback","1-mix-2","delay 2","feedback","rate","rate","rate"
};
const char* const name24[] PROGMEM = {
 str24[0], str24[1], str24[2], str24[3], str24[4], str24[5], str24[6], str24[7],
};

//ROM2 param3
const char str25[8][12] PROGMEM = {//str1 is effect name of 1st line
"filter","phaser","pitch 2","delay 3","delay","wah","vibrato","phaser"
};
const char* const name25[] PROGMEM = {
 str25[0], str25[1], str25[2], str25[3], str25[4], str25[5], str25[6], str25[7],
};

//ROM3 1st line
const char str31[8][12] PROGMEM = {//str1 is effect name of 1st line
"Phaser","Flanger","Triple","6 Tap","Cabinet","Parallax","Choir","Spacedash"
};
const char* const name31[] PROGMEM = {
 str31[0], str31[1], str31[2], str31[3], str31[4], str31[5], str31[6], str31[7],
};

//ROM3 2nd line
const char str32[8][12] PROGMEM = {//str1 is effect name of 1st line
"Bass","Bass","Delay","Delay","Simulator","Phaser","Saw",""
};
const char* const name32[] PROGMEM = {
 str32[0], str32[1], str32[2], str32[3], str32[4], str32[5], str32[6], str32[7],
};

//ROM3 param1
const char str33[8][12] PROGMEM = {//str1 is effect name of 1st line
"rate","rate","delay 1","delay time","low pass","rate","delay","intensity"
};
const char* const name33[] PROGMEM = {
 str33[0], str33[1], str33[2], str33[3], str33[4], str33[5], str33[6], str33[7],
};

//ROM3 param2
const char str34[8][12] PROGMEM = {//str1 is effect name of 1st line
"depth","depth","delay 2","flanger lfo","high pass","depth","mix","rate"
};
const char* const name34[] PROGMEM = {
 str34[0], str34[1], str34[2], str34[3], str34[4], str34[5], str34[6], str34[7],
};

//ROM2 param3
const char str35[8][12] PROGMEM = {//str1 is effect name of 1st line
"feedback","feedback","delay 3","feedback","volume","feedback","volume","mix"
};
const char* const name35[] PROGMEM = {
 str35[0], str35[1], str35[2], str35[3], str35[4], str35[5], str35[6], str35[7],
};
