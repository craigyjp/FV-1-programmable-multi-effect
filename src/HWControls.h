#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <Bounce.h>
#include "TButton.h"

// #include <ADC.h>
// #include <ADC_util.h>

// ADC *adc = new ADC();

#define MUX1_S A0
#define MUX2_S A1
#define MUX3_S A2
#define MUX4_S A3
#define MUX5_S A4

#define RECALL_SW 10
#define ENCODER_PINA 19
#define ENCODER_PINB 18

#define SDCARD 20
#define DAC_CS1 11

#define BACK_SW 15
#define SAVE_SW 16
#define SETTINGS_SW 17

#define BANK0_SW 27
#define BANK1_SW 26
#define BANK2_SW 25
#define BANK3_SW 24

#define BANK0_LED 0
#define BANK1_LED 1
#define BANK2_LED 2
#define BANK3_LED 3
#define INTERNAL 4
#define EEPROM_1 5
#define EEPROM_2 6
#define EEPROM_3 7

#define PROGRAM_0 8
#define PROGRAM_1 9
#define PROGRAM_2 10

#define SEG_DIO 4
#define SEG_CLK 5


#define DEBOUNCE 30

#define QUANTISE_FACTOR 7

static int mux1ValuesPrev;
static int mux2ValuesPrev;
static int mux3ValuesPrev;
static int mux4ValuesPrev;
static int mux5ValuesPrev;
static int mux1Read = 0;
static int mux2Read = 0;
static int mux3Read = 0;
static int mux4Read = 0;
static int mux5Read = 0;

static long encPrevious = 0;

//These are pushbuttons and require debouncing
TButton BANK0_Switch{BANK0_SW, LOW, HOLD_DURATION, DEBOUNCE, CLICK_DURATION};
TButton BANK1_Switch{BANK1_SW, LOW, HOLD_DURATION, DEBOUNCE, CLICK_DURATION};
TButton BANK2_Switch{BANK2_SW, LOW, HOLD_DURATION, DEBOUNCE, CLICK_DURATION};
TButton BANK3_Switch{BANK3_SW, LOW, HOLD_DURATION, DEBOUNCE, CLICK_DURATION};

TButton saveButton{SAVE_SW, LOW, HOLD_DURATION, DEBOUNCE, CLICK_DURATION};
TButton settingsButton{SETTINGS_SW, LOW, HOLD_DURATION, DEBOUNCE, CLICK_DURATION};
TButton backButton{BACK_SW, LOW, HOLD_DURATION, DEBOUNCE, CLICK_DURATION};
TButton recallButton{RECALL_SW, LOW, HOLD_DURATION, DEBOUNCE, CLICK_DURATION}; //On encoder
                        
Encoder encoder(ENCODER_PINB, ENCODER_PINA);        //This often needs the pins swapping depending on the encoder

void setupHardware() {

  //Switches

  pinMode(DAC_CS1, OUTPUT);

  digitalWrite(DAC_CS1, HIGH);

  pinMode(RECALL_SW, INPUT_PULLUP);  //On encoder
  pinMode(SAVE_SW, INPUT_PULLUP);
  pinMode(SETTINGS_SW, INPUT_PULLUP);
  pinMode(BACK_SW, INPUT_PULLUP);

  pinMode(BANK0_SW, INPUT_PULLUP);
  pinMode(BANK1_SW, INPUT_PULLUP);
  pinMode(BANK2_SW, INPUT_PULLUP);
  pinMode(BANK3_SW, INPUT_PULLUP);

  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE1));
  digitalWrite(DAC_CS1, LOW);
  SPI.transfer(int_ref_on_flexible_mode >> 24);
  SPI.transfer(int_ref_on_flexible_mode >> 16);
  SPI.transfer(int_ref_on_flexible_mode >> 8);
  SPI.transfer(int_ref_on_flexible_mode);
  delayMicroseconds(1);
  digitalWrite(DAC_CS1, HIGH);
  SPI.endTransaction();

}
