
#include <avr/pgmspace.h>
#include <Wire.h>
#include "effects.h"

#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
ExternalEEPROM myMem;

#include <AceButton.h>
using namespace ace_button;

const int BUTTON_PIN = 2;
AceButton button(BUTTON_PIN);

void handleEvent(AceButton*, uint8_t, uint8_t);

#define PRESETLEN 512

void readpreset(uint8_t preset) {

  uint32_t offset = preset*PRESETLEN;
  char buf[10];

  Serial.print("Reading preset: ");
  Serial.print(preset);
  Serial.print(" memory offset: ");
  Serial.println(offset);
  Serial.println("");
  
  Serial.print("const uint8_t preset");
  Serial.print(preset);
  Serial.println("[] PROGMEM = {");
  for ( uint32_t x=offset; x < offset+PRESETLEN; x = x+16) {
    for( uint32_t y=0; y<16; y++) {
        byte val = myMem.read(x+y);
        sprintf(buf, "0x%02X, ", val);        
        Serial.print(buf);
    }
    Serial.println("");
  }
  Serial.println("};");
}

void writepreset(uint8_t preset, const uint8_t *data, size_t len) {
  
  uint32_t offset = preset*PRESETLEN;
  
  Serial.print("Writing preset: ");
  Serial.print(preset);
  Serial.print(" memory offset: ");
  Serial.print(offset);
  Serial.print(" length: ");
  Serial.println(len);

  for( size_t i=0; i<len; i++) {
    myMem.write(offset+i, pgm_read_byte(data+i));   
  }
}

void setup() {
  Serial.begin(115200);

  pinMode (BUTTON_PIN, INPUT_PULLUP); // push button on pin 2
  
  // Get the current config.
  ButtonConfig* config = button.getButtonConfig();
  config->setFeature(ButtonConfig::kFeatureLongPress);
  config->setFeature(ButtonConfig::kFeatureClick);
  config->setEventHandler(handleEvent);

  Wire.begin();
  Wire.setClock(400000); //Most EEPROMs can run 400kHz and higher

  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1);
  }

  Serial.println("Memory detected!");
  
  // 24LC32A
  myMem.setMemorySize(32768/8); //In bytes. 32kbits 4kbytes
  myMem.setPageSize(32); //In bytes. Has 32 byte page size.
  myMem.enablePollForWriteComplete(); //Supports I2C polling of write completion
  myMem.setPageWriteTime(5); //5 ms max write time

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Click on the button to read the eeprom");
  Serial.println("Long press the button to write the eeprom");
}

bool done = false;

void loop() {
  if (done) {
    // blink when dump writen
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(500);
    digitalWrite(LED_BUILTIN, LOW); 
    delay(500); 
  } else {
    button.check();
  }
}

void handleEvent(AceButton* /*button*/, uint8_t eventType,
    uint8_t /*buttonState*/) {
    switch (eventType) {
      case AceButton::kEventClicked:
          Serial.println("Start read");
          readpreset(0);
          readpreset(1);
          readpreset(2);
          readpreset(3);
          readpreset(4);
          readpreset(5);
          readpreset(6);
          readpreset(7);
            
          done = true;
          Serial.println("Done, reset the board to start again");
          break;
      case AceButton::kEventLongPressed:
          Serial.println("Start write");
          
          writepreset(0, phaserbass, PRESETLEN);
          writepreset(1, flangerbass, PRESETLEN);
          writepreset(2, vp330, PRESETLEN);
          writepreset(3, cathedral, PRESETLEN);
          writepreset(4, rotorme2009, PRESETLEN);
          writepreset(5, ensembleme2009, PRESETLEN);
          writepreset(6, leslieme2009, PRESETLEN);
          writepreset(7, wahwah, PRESETLEN);
          
          done = true;
          Serial.println("Done, reset the board to start again");
          break;
    }
}
    
