// Constants
const float LFOTEMPO[128] = {0.050, 0.050, 0.055, 0.055, 0.060, 0.064, 0.064, 0.069, 0.072, 0.077, 0.077, 0.081, 0.080, 0.087, 0.087, 0.092, 0.100, 0.100, 0.100, 0.104, 0.109, 0.115, 0.115, 0.122, 0.130, 0.130, 0.139, 0.149, 0.160, 0.160, 0.172, 0.185, 0.196, 0.20, 0.20, 0.21, 0.22, 0.24, 0.24, 0.26, 0.27, 0.29, 0.29, 0.31, 0.34, 0.37, 0.37, 0.39, 0.4, 0.40, 0.42, 0.45, 0.47, 0.47, 0.50, 0.53, 0.57, 0.57, 0.6, 0.62, 0.62, 0.65, 0.69, 0.74, 0.74, 0.80, 0.80, 0.86, 0.86, 0.94, 1.00, 1.00, 1.10, 1.12, 1.14, 1.14, 1.17, 1.20, 1.30, 1.40, 1.50, 1.50, 1.60, 1.60, 1.7, 1.8, 1.9, 2.0, 2.2, 2.2, 2.4, 2.6, 2.8, 3.00, 3.00, 3.1, 3.2, 3.2, 3.4, 3.80, 3.80, 4.10, 4.40, 4.70, 4.70, 5.00, 5.40, 5.80, 6.00, 6.00, 6.20, 6.40, 6.40, 6.80, 6.80, 7.30, 7.60, 8.00, 8.60, 9.30, 9.90, 10.60, 10.60, 11.50, 12.20, 12.80, 12.80, 12.80};
#define RE_READ -9
#define NO_OF_PARAMS 15
const char* INITPATCHNAME = "Initial Patch";
#define HOLD_DURATION 1500
const uint32_t CLICK_DURATION = 250;
#define PATCHES_LIMIT 128
const String INITPATCH = "Solina,1,1,1,1,1,1,1,1,1,1,1";
const char* VERSION = "Version 1.1";

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

int Patchnumber = 0;
unsigned long timeout = 0;
unsigned long firsttimer = 0;

String patchName = INITPATCHNAME;
boolean encCW = true;//This is to set the encoder to increment when turned CW - Settings Option

//Values below are just for initialising and will be changed when synth is initialised to current panel controls & EEPROM settings
byte midiChannel = MIDI_CHANNEL_OMNI;//(EEPROM)
boolean announce = false;

String StratusLFOWaveform = "                ";
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

int DAC0 = 0;

int returnvalue = 0;

uint32_t int_ref_on_flexible_mode = 0b00001001000010100000000000000000;  // { 0000 , 1001 , 0000 , 1010000000000000 , 0000 }

uint32_t sample_data = 0b00000000000000000000000000000000;

uint32_t sample_data1 = 0b00000000000000000000000000000000;
uint32_t sample_data2 = 0b00000000000000000000000000000000;
uint32_t sample_data3 = 0b00000000000000000000000000000000;
uint32_t sample_data4 = 0b00000000000000000000000000000000;
uint32_t sample_data5 = 0b00000000000000000000000000000000;
uint32_t sample_data6 = 0b00000000000000000000000000000000;
uint32_t sample_data7 = 0b00000000000000000000000000000000;
uint32_t sample_data8 = 0b00000000000000000000000000000000;

uint32_t channel_a = 0b00000010000000000000000000000000;
uint32_t channel_b = 0b00000010000100000000000000000000;
uint32_t channel_c = 0b00000010001000000000000000000000;
uint32_t channel_d = 0b00000010001100000000000000000000;
uint32_t channel_e = 0b00000010010000000000000000000000;
uint32_t channel_f = 0b00000010010100000000000000000000;
uint32_t channel_g = 0b00000010011000000000000000000000;
uint32_t channel_h = 0b00000010011100000000000000000000;
