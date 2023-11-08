#define SETTINGSOPTIONSNO 4
#define SETTINGSVALUESNO 18  //Maximum number of settings option values needed
int settingsValueIndex = 0;  //currently selected settings option value index

struct SettingsOption {
  char *option;                   //Settings option string
  char *value[SETTINGSVALUESNO];  //Array of strings of settings option values
  int handler;                    //Function to handle the values for this settings option
  int currentIndex;               //Function to array index of current value for this settings option
};

void settingsMIDICh(char *value);
void settingsEncoderDir(char * value);
void settingsSendParams(char * value);
void settingsLEDintensity(char * value);
void settingsHandler(char *s, void (*f)(char *));

int currentIndexMIDICh();
int currentIndexEncoderDir();
int currentIndexSendParams();
int currentIndexLEDintensity();
int getCurrentIndex(int (*f)());


void settingsMIDICh(char *value) {
  if (strcmp(value, "ALL") == 0) {
    midiChannel = MIDI_CHANNEL_OMNI;
  } else {
    midiChannel = atoi(value);
  }
  storeMidiChannel(midiChannel);
}

void settingsEncoderDir(char * value) {
  if (strcmp(value, "Type 1") == 0) {
    encCW = true;
  } else {
    encCW =  false;
  }
  storeEncoderDir(encCW ? 1 : 0);
}

void settingsSendParams(char * value) {
  if (strcmp(value, "Off") == 0) {
    updateParams = false;
  } else {
    updateParams =  true;
  }
  storeSendParams(updateParams ? 0 : 1);
}

void settingsLEDintensity(char *value) {
  if (strcmp(value, "Off") == 0) {
    LEDintensity = 0;
  } else {
    LEDintensity = atoi(value);
  }
  storeLEDintensity(LEDintensity);
}

//Takes a pointer to a specific method for the settings option and invokes it.
void settingsHandler(char *s, void (*f)(char *)) {
  f(s);
}

int currentIndexMIDICh() {
  return getMIDIChannel();
}

int currentIndexEncoderDir() {
  return getEncoderDir() ? 0 : 1;
}

int currentIndexSendParams() {
  return getSendParams() ? 0 : 1;
}

int currentIndexLEDintensity() {
  return getLEDintensity();
}

//Takes a pointer to a specific method for the current settings option value and invokes it.
int getCurrentIndex(int (*f)()) {
  return f();
}

CircularBuffer<SettingsOption, SETTINGSOPTIONSNO> settingsOptions;

// add settings to the circular buffer
void setUpSettings() {
  settingsOptions.push(SettingsOption{"MIDI Ch.", { "All", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", '\0' }, settingsMIDICh, currentIndexMIDICh });
  settingsOptions.push(SettingsOption{"Encoder", {"Type 1", "Type 2", '\0'}, settingsEncoderDir, currentIndexEncoderDir});
  settingsOptions.push(SettingsOption{"Send Params", {"Off", "on", '\0'}, settingsSendParams, currentIndexSendParams});
  settingsOptions.push(SettingsOption{"LED Bright", { "Off", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", '\0' }, settingsLEDintensity, currentIndexLEDintensity });
}
