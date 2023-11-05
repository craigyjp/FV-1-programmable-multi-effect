#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <MIDI.h>
#include "Parameters.h"
#include <CircularBuffer.h>
#include "PatchMgr.h"
#include "HWControls.h"
#include "EepromMgr.h"
#include "Settings.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ShiftRegister74HC595.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>

#define PARAMETER 0      //The main page for displaying the current patch and control (parameter) changes
#define RECALL 1         //Patches list
#define SAVE 2           //Save patch page
#define REINITIALISE 3   // Reinitialise message
#define PATCH 4          // Show current patch bypassing PARAMETER
#define PATCHNAMING 5    // Patch naming page
#define DELETE 6         //Delete patch page
#define DELETEMSG 7      //Delete patch message page
#define SETTINGS 8       //Settings page
#define SETTINGSVALUE 9  //Settings page

unsigned int state = PARAMETER;

#include "ST7735Display.h"

int patchNo = 1;
boolean cardStatus = false;

unsigned long buttonDebounce = 0;

//MIDI 5 Pin DIN
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  //RX - Pin 0

// create a global shift register object
// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<2> sr(45, 43, 44);

// LED displays
SevenSegmentExtended patchDisplay(SEG_CLK, SEG_DIO);

void setup() {

  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // OLED I2C Address, may need to change for different device,
  setUpSettings();

  setupHardware();
  renderBootUpPage();

  patchDisplay.begin();            // initializes the display
  patchDisplay.setBacklight(100);  // set the brightness to 100 %
  padPatchNumber();
  patchDisplay.print(patchNumber);  // display INIT on the display
  delay(10);

  cardStatus = SD.begin(SDCARD);
  if (cardStatus) {

    //Get patch numbers and names from SD card
    loadPatches();
    if (patches.size() == 0) {
      //save an initialised patch to SD card
      savePatch("1", INITPATCH);
      loadPatches();
    }
  } else {

    reinitialiseToPanel();
    showPatchPage("No SD", "conn'd / usable");
  }

  //Read MIDI Channel from EEPROM

  midiChannel = getMIDIChannel();
  if (midiChannel < 0 || midiChannel > 16) {
    storeMidiChannel(0);
  }

  //MIDI 5 Pin DIN
  MIDI.begin();
  MIDI.setHandleControlChange(myConvertControlChange);
  MIDI.setHandleProgramChange(myProgramChange);

  //Read Encoder Direction from EEPROM
  encCW = getEncoderDir();
  startFirstTimer();
  recallPatch(patchNo);
  updateScreen();
}

void padPatchNumber() {
  if (patchNo < 10) {
    patchNumber = "  " + String(patchNo);
  }
  if (patchNo < 100 && patchNo > 9) {
    patchNumber = " " + String(patchNo);
  }
}

void startFirstTimer() {
  firsttimer = millis();
}

void outputDAC(int CHIP_SELECT, uint32_t sample_data) {
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE1));
  digitalWrite(CHIP_SELECT, LOW);
  SPI.transfer(sample_data >> 24);
  SPI.transfer(sample_data >> 16);
  SPI.transfer(sample_data >> 8);
  SPI.transfer(sample_data);
  delayMicroseconds(8);  // Settling time delay
  digitalWrite(CHIP_SELECT, HIGH);
  SPI.endTransaction();
}

void myConvertControlChange(byte channel, byte number, byte value) {
  int newvalue = value << 3;
  myControlChange(channel, number, newvalue);
}

void myProgramChange(byte channel, byte program) {
  state = PATCH;
  patchNo = program + 1;
  recallPatch(patchNo);
  updateScreen();
  state = PARAMETER;
}

void recallPatch(int patchNo) {
  File patchFile = SD.open(String(patchNo).c_str());
  if (!patchFile) {
  } else {
    String data[NO_OF_PARAMS];  //Array of data read in
    recallPatchData(patchFile, data);
    setCurrentPatchData(data);
    patchFile.close();
    padPatchNumber();
    patchDisplay.print(patchNumber); 
  }
}

void setCurrentPatchData(String data[]) {
  patchName = data[0];
  effect = data[1].toInt();
  pot1 = data[2].toInt();
  pot2 = data[3].toInt();
  pot3 = data[4].toInt();
  mix = data[5].toInt();
  bank0 = data[6].toInt();
  bank1 = data[7].toInt();
  bank2 = data[8].toInt();
  bank3 = data[9].toInt();

  //Switches
  updatebank(0);
  updateeffect(0);
  updatepot1(0);
  updatepot2(0);
  updatepot3(0);
  updatemix(0);

  //Patchname
  updatePatchname();
}

String getCurrentPatchData() {
  return patchName + "," + String(effect) + "," + String(pot1) + "," + String(pot2) + "," + String(pot3) + "," + String(mix) + "," + String(bank0) + "," + String(bank1) + "," + String(bank2) + "," + String(bank3);
}

void updatePatchname() {
  showPatchPage(String(patchNo), patchName);
}

void updateeffect(boolean announce) {
  switch (effect) {
    case 0:
      sr.set(PROGRAM_0, LOW);
      sr.set(PROGRAM_1, LOW);
      sr.set(PROGRAM_2, LOW);
      i = 0;
      break;

    case 1:
      sr.set(PROGRAM_0, HIGH);
      sr.set(PROGRAM_1, LOW);
      sr.set(PROGRAM_2, LOW);
      i = 1;
      break;

    case 2:
      sr.set(PROGRAM_0, LOW);
      sr.set(PROGRAM_1, HIGH);
      sr.set(PROGRAM_2, LOW);
      i = 2;
      break;

    case 3:
      sr.set(PROGRAM_0, HIGH);
      sr.set(PROGRAM_1, HIGH);
      sr.set(PROGRAM_2, LOW);
      i = 3;
      break;

    case 4:
      sr.set(PROGRAM_0, LOW);
      sr.set(PROGRAM_1, LOW);
      sr.set(PROGRAM_2, HIGH);
      i = 4;
      break;

    case 5:
      sr.set(PROGRAM_0, HIGH);
      sr.set(PROGRAM_1, LOW);
      sr.set(PROGRAM_2, HIGH);
      i = 5;
      break;

    case 6:
      sr.set(PROGRAM_0, LOW);
      sr.set(PROGRAM_1, HIGH);
      sr.set(PROGRAM_2, HIGH);
      i = 6;
      break;

    case 7:
      sr.set(PROGRAM_0, HIGH);
      sr.set(PROGRAM_1, HIGH);
      sr.set(PROGRAM_2, HIGH);
      i = 7;
      break;
  }
}

void updatepot1(boolean announce) {
  uint32_t outputValue_a = map(pot1, 0, 1023, 0, 43785);
  sample_data = ((channel_a & 0xFFF0000F) | (outputValue_a & 0xFFFF) << 4);
  outputDAC(DAC_CS1, sample_data);
}

void updatepot2(boolean announce) {
  uint32_t outputValue_b = map(pot2, 0, 1023, 0, 43785);
  sample_data = ((channel_b & 0xFFF0000F) | (outputValue_b & 0xFFFF) << 4);
  outputDAC(DAC_CS1, sample_data);
}

void updatepot3(boolean announce) {
  uint32_t outputValue_c = map(pot3, 0, 1023, 0, 43785);
  sample_data = ((channel_c & 0xFFF0000F) | (outputValue_c & 0xFFFF) << 4);
  outputDAC(DAC_CS1, sample_data);
}

void updatemix(boolean announce) {
  uint32_t outputValue_e = map(mix, 0, 1023, 0, 52428);
  sample_data = ((channel_e & 0xFFF0000F) | (outputValue_e & 0xFFFF) << 4);
  outputDAC(DAC_CS1, sample_data);

  uint32_t outputValue_f = map(mix, 0, 1023, 52428, 0);
  sample_data = ((channel_f & 0xFFF0000F) | (outputValue_f & 0xFFFF) << 4);
  outputDAC(DAC_CS1, sample_data);
}

void updatebank(boolean announce) {
  if (bank0) {
    sr.set(BANK0_LED, HIGH);
    sr.set(BANK1_LED, LOW);
    sr.set(BANK2_LED, LOW);
    sr.set(BANK3_LED, LOW);
    sr.set(INTERNAL, LOW);
    sr.set(EEPROM_1, HIGH);
    sr.set(EEPROM_2, HIGH);
    sr.set(EEPROM_3, HIGH);
  }
  if (bank1) {
    sr.set(BANK0_LED, LOW);
    sr.set(BANK1_LED, HIGH);
    sr.set(BANK2_LED, LOW);
    sr.set(BANK3_LED, LOW);
    sr.set(INTERNAL, HIGH);
    sr.set(EEPROM_1, LOW);
    sr.set(EEPROM_2, HIGH);
    sr.set(EEPROM_3, HIGH);
  }
  if (bank2) {
    sr.set(BANK0_LED, LOW);
    sr.set(BANK1_LED, LOW);
    sr.set(BANK2_LED, HIGH);
    sr.set(BANK3_LED, LOW);
    sr.set(INTERNAL, HIGH);
    sr.set(EEPROM_1, HIGH);
    sr.set(EEPROM_2, LOW);
    sr.set(EEPROM_3, HIGH);
  }
  if (bank3) {
    sr.set(BANK0_LED, LOW);
    sr.set(BANK1_LED, LOW);
    sr.set(BANK2_LED, LOW);
    sr.set(BANK3_LED, HIGH);
    sr.set(INTERNAL, HIGH);
    sr.set(EEPROM_1, HIGH);
    sr.set(EEPROM_2, HIGH);
    sr.set(EEPROM_3, LOW);
  }
}

void myControlChange(byte channel, byte control, int value) {

  switch (control) {

    case CCeffect:
      effect = map(value, 0, 1023, 0, 7);
      updateeffect(1);
      break;

    case CCpot1:
      pot1str = map(value, 0, 1023, 0, 127);  // for display
      pot1 = value;
      updatepot1(1);
      break;

    case CCpot2:
      pot2str = map(value, 0, 1023, 0, 127);  // for display
      pot2 = value;
      updatepot2(1);
      break;

    case CCpot3:
      pot3str = map(value, 0, 1023, 0, 127);  // for display
      pot3 = value;
      updatepot3(1);
      break;

    case CCmix:
      mixstra = map(value, 0, 1023, 0, 127);  // for display
      mixstrb = map(value, 0, 1023, 127, 0);  // for display
      mix = value;
      updatemix(1);
      break;

    case CCbank0:
      updatebank(1);
      break;

    case CCbank1:
      updatebank(1);
      break;

    case CCbank2:
      updatebank(1);
      break;

    case CCbank3:
      updatebank(1);
      break;
  }
}

void checkMux() {

  mux1Read = analogRead(MUX1_S);
  if (mux1Read > (mux1ValuesPrev + QUANTISE_FACTOR) || mux1Read < (mux1ValuesPrev - QUANTISE_FACTOR)) {

    mux1ValuesPrev = mux1Read;
    myControlChange(midiChannel, CCpot3, mux1Read);
  }

  mux2Read = analogRead(MUX2_S);
  if (mux2Read > (mux2ValuesPrev + QUANTISE_FACTOR) || mux2Read < (mux2ValuesPrev - QUANTISE_FACTOR)) {

    mux2ValuesPrev = mux2Read;
    myControlChange(midiChannel, CCpot2, mux2Read);
  }

  mux3Read = analogRead(MUX3_S);
  if (mux3Read > (mux3ValuesPrev + QUANTISE_FACTOR) || mux3Read < (mux3ValuesPrev - QUANTISE_FACTOR)) {

    mux3ValuesPrev = mux3Read;
    myControlChange(midiChannel, CCeffect, mux3Read);
  }

  mux4Read = analogRead(MUX4_S);
  if (mux4Read > (mux4ValuesPrev + QUANTISE_FACTOR) || mux4Read < (mux4ValuesPrev - QUANTISE_FACTOR)) {

    mux4ValuesPrev = mux4Read;
    myControlChange(midiChannel, CCpot1, mux4Read);
  }

  mux5Read = analogRead(MUX5_S);
  if (mux5Read > (mux5ValuesPrev + QUANTISE_FACTOR) || mux5Read < (mux5ValuesPrev - QUANTISE_FACTOR)) {

    mux5ValuesPrev = mux5Read;
    myControlChange(midiChannel, CCmix, mux5Read);
  }
}

void writeDemux() {
}

void reinitialiseToPanel() {
  //This sets the current patch to be the same as the current hardware panel state - all the pots
  //The four button controls stay the same state
  //This reinialises the previous hardware values to force a re-read
  startFirstTimer();
  mux1ValuesPrev = RE_READ;
  mux2ValuesPrev = RE_READ;
  mux3ValuesPrev = RE_READ;
  mux4ValuesPrev = RE_READ;
  mux5ValuesPrev = RE_READ;

  patchName = INITPATCHNAME;
  showPatchPage("Initial", "Panel Settings");
}

void checkSwitches() {

  BANK0_Switch.update();
  if (BANK0_Switch.numClicks() == 1) {
    if (!bank0) {
      bank0 = 1;
      bank1 = 0;
      bank2 = 0;
      bank3 = 0;
      myControlChange(midiChannel, CCbank0, bank0);
    }
  }

  BANK1_Switch.update();
  if (BANK1_Switch.numClicks() == 1) {
    if (!bank1) {
      bank0 = 0;
      bank1 = 1;
      bank2 = 0;
      bank3 = 0;
      myControlChange(midiChannel, CCbank1, bank1);
    }
  }

  BANK2_Switch.update();
  if (BANK2_Switch.numClicks() == 1) {
    if (!bank2) {
      bank0 = 0;
      bank1 = 0;
      bank2 = 1;
      bank3 = 0;
      myControlChange(midiChannel, CCbank2, bank2);
    }
  }

  BANK3_Switch.update();
  if (BANK3_Switch.numClicks() == 1) {
    if (!bank3) {
      bank0 = 0;
      bank1 = 0;
      bank2 = 0;
      bank3 = 1;
      myControlChange(midiChannel, CCbank3, bank3);
    }
  }

  saveButton.update();
  if (saveButton.held()) {
    switch (state) {
      case PARAMETER:
      case PATCH:
        state = DELETE;
        updateScreen();
        break;
    }
  } else if (saveButton.numClicks() == 1) {
    switch (state) {
      case PARAMETER:
        if (patches.size() < PATCHES_LIMIT) {
          resetPatchesOrdering();  //Reset order of patches from first patch
          patches.push({ patches.size() + 1, INITPATCHNAME });
          state = SAVE;
        }
        updateScreen();
        break;
      case SAVE:
        //Save as new patch with INITIALPATCH name or overwrite existing keeping name - bypassing patch renaming
        patchName = patches.last().patchName;
        state = PATCH;
        savePatch(String(patches.last().patchNo).c_str(), getCurrentPatchData());
        showPatchPage(patches.last().patchNo, patches.last().patchName);
        patchNo = patches.last().patchNo;
        loadPatches();  //Get rid of pushed patch if it wasn't saved
        setPatchesOrdering(patchNo);
        renamedPatch = "";
        state = PARAMETER;
        updateScreen();
        break;
      case PATCHNAMING:
        if (renamedPatch.length() > 0) patchName = renamedPatch;  //Prevent empty strings
        state = PATCH;
        savePatch(String(patches.last().patchNo).c_str(), getCurrentPatchData());
        showPatchPage(patches.last().patchNo, patchName);
        patchNo = patches.last().patchNo;
        loadPatches();  //Get rid of pushed patch if it wasn't saved
        setPatchesOrdering(patchNo);
        renamedPatch = "";
        state = PARAMETER;
        updateScreen();
        break;
    }
  }

  settingsButton.update();
  if (settingsButton.held()) {
    //If recall held, set current patch to match current hardware state
    //Reinitialise all hardware values to force them to be re-read if different
    state = REINITIALISE;
    reinitialiseToPanel();
    updateScreen();  //Hack
  } else if (settingsButton.numClicks() == 1) {
    switch (state) {
      case PARAMETER:
        settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        state = SETTINGS;
        updateScreen();
        break;
      case SETTINGS:
        settingsOptions.push(settingsOptions.shift());
        settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        //updateScreen();
      case SETTINGSVALUE:
        //Same as pushing Recall - store current settings item and go back to options
        settingsHandler(settingsOptions.first().value[settingsValueIndex], settingsOptions.first().handler);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        state = SETTINGS;
        updateScreen();
        break;
    }
  }

  backButton.update();
  if (backButton.held()) {
    //If Back button held, Panic - all notes off
    updateScreen();  //Hack
  } else if (backButton.numClicks() == 1) {
    switch (state) {
      case RECALL:
        setPatchesOrdering(patchNo);
        state = PARAMETER;
        updateScreen();
        break;
      case SAVE:
        renamedPatch = "";
        state = PARAMETER;
        loadPatches();  //Remove patch that was to be saved
        setPatchesOrdering(patchNo);
        updateScreen();
        break;
      case PATCHNAMING:
        charIndex = 0;
        renamedPatch = "";
        state = SAVE;
        updateScreen();
        break;
      case DELETE:
        setPatchesOrdering(patchNo);
        state = PARAMETER;
        updateScreen();
        break;
      case SETTINGS:
        state = PARAMETER;
        updateScreen();
        break;

      case SETTINGSVALUE:
        settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        state = SETTINGS;
        updateScreen();
        break;
    }
  }

  //Encoder switch
  recallButton.update();
  if (recallButton.held()) {
    //If Recall button held, return to current patch setting
    //which clears any changes made
    state = PATCH;
    //Recall the current patch
    patchNo = patches.first().patchNo;
    recallPatch(patchNo);
    state = PARAMETER;
    updateScreen();  //Hack
  } else if (recallButton.numClicks() == 1) {
    switch (state) {
      case PARAMETER:
        state = RECALL;  //show patch list
        updateScreen();
        break;
      case RECALL:
        state = PATCH;
        //Recall the current patch
        patchNo = patches.first().patchNo;
        recallPatch(patchNo);
        state = PARAMETER;
        updateScreen();
        break;
      case SAVE:
        showRenamingPage(patches.last().patchName);
        patchName = patches.last().patchName;
        state = PATCHNAMING;
        updateScreen();
        break;
      case PATCHNAMING:
        if (renamedPatch.length() < 12)  //actually 12 chars
        {
          renamedPatch.concat(String(currentCharacter));
          charIndex = 0;
          currentCharacter = CHARACTERS[charIndex];
          showRenamingPage(renamedPatch);
        }
        updateScreen();
        break;
      case DELETE:
        //Don't delete final patch
        if (patches.size() > 1) {
          state = DELETEMSG;
          patchNo = patches.first().patchNo;     //PatchNo to delete from SD card
          patches.shift();                       //Remove patch from circular buffer
          deletePatch(String(patchNo).c_str());  //Delete from SD card
          loadPatches();                         //Repopulate circular buffer to start from lowest Patch No
          renumberPatchesOnSD();
          loadPatches();                      //Repopulate circular buffer again after delete
          patchNo = patches.first().patchNo;  //Go back to 1
          recallPatch(patchNo);               //Load first patch
        }
        state = PARAMETER;
        updateScreen();
        break;
      case SETTINGS:
        //Choose this option and allow value choice
        settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGSVALUE);
        state = SETTINGSVALUE;
        updateScreen();
        break;
      case SETTINGSVALUE:
        //Store current settings item and go back to options
        settingsHandler(settingsOptions.first().value[settingsValueIndex], settingsOptions.first().handler);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        state = SETTINGS;
        updateScreen();
        break;
    }
  }
}

void checkEncoder() {
  //Encoder works with relative inc and dec values
  //Detent encoder goes up in 4 steps, hence +/-3

  long encRead = encoder.read();
  if ((encCW && encRead > encPrevious + 3) || (!encCW && encRead < encPrevious - 3)) {
    switch (state) {
      case PARAMETER:
        state = PATCH;
        patches.push(patches.shift());
        patchNo = patches.first().patchNo;
        recallPatch(patchNo);
        state = PARAMETER;
        updateScreen();
        break;
      case RECALL:
        patches.push(patches.shift());
        updateScreen();
        break;
      case SAVE:
        patches.push(patches.shift());
        updateScreen();
        break;
      case PATCHNAMING:
        if (charIndex == TOTALCHARS) charIndex = 0;  //Wrap around
        currentCharacter = CHARACTERS[charIndex++];
        showRenamingPage(renamedPatch + currentCharacter);
        updateScreen();
        break;
      case DELETE:
        patches.push(patches.shift());
        updateScreen();
        break;
      case SETTINGS:
        settingsOptions.push(settingsOptions.shift());
        settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        updateScreen();
        break;
      case SETTINGSVALUE:
        if (settingsOptions.first().value[settingsValueIndex + 1] != '\0')
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[++settingsValueIndex], SETTINGSVALUE);
        updateScreen();
        break;
    }
    encPrevious = encRead;
  } else if ((encCW && encRead < encPrevious - 3) || (!encCW && encRead > encPrevious + 3)) {
    switch (state) {
      case PARAMETER:
        state = PATCH;
        patches.unshift(patches.pop());
        patchNo = patches.first().patchNo;
        recallPatch(patchNo);
        state = PARAMETER;
        updateScreen();
        break;
      case RECALL:
        patches.unshift(patches.pop());
        updateScreen();
        break;
      case SAVE:
        patches.unshift(patches.pop());
        updateScreen();
        break;
      case PATCHNAMING:
        if (charIndex == -1)
          charIndex = TOTALCHARS - 1;
        currentCharacter = CHARACTERS[charIndex--];
        showRenamingPage(renamedPatch + currentCharacter);
        updateScreen();
        break;
      case DELETE:
        patches.unshift(patches.pop());
        updateScreen();
        break;
      case SETTINGS:
        settingsOptions.unshift(settingsOptions.pop());
        settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        updateScreen();
        break;
      case SETTINGSVALUE:
        if (settingsValueIndex > 0)
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[--settingsValueIndex], SETTINGSVALUE);
        updateScreen();
        break;
    }
    encPrevious = encRead;
  }
}

void loop() {
  MIDI.read();
  updateScreen();
  checkSwitches();
  checkEncoder();
  checkMux();
  writeDemux();
}
