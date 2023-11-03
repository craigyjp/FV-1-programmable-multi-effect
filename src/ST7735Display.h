#define DISPLAYTIMEOUT 1500

#include <Fonts/Org_01.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#define OLED_RESET 7      // Reset pin # (or -1 if sharing Arduino reset pin
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String currentParameter = "";
String currentValue = "";
float currentFloatValue = 0.0;
String currentPgmNum = "";
String currentPatchName = "";
String newPatchName = "";
char * currentSettingsOption = "";
char * currentSettingsValue = "";
int currentSettingsPart = SETTINGS;
int paramType = PARAMETER;

boolean MIDIClkSignal = false;

unsigned long timer = 0;

void startTimer()
{
  if (state == PARAMETER)
  {
    timer = millis();
  }
}

void renderBootUpPage() {
  startTimer();
  display.clearDisplay();
  display.setFont(&Org_01);
  display.setTextColor(WHITE, BLACK);
  display.drawRect(5, 15, 35, 15, WHITE);
  display.fillRect(40, 15, 87, 15, WHITE);
  display.setCursor(10, 24);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.print("MFOS");
  display.setCursor(48, 24);
  display.setTextColor(BLACK, WHITE);
  display.print("16 STG PHASER");
  display.setTextColor(WHITE, BLACK);
  display.drawRect(5, 45, 72, 15, WHITE);
  display.fillRect(77, 45, 54, 15, WHITE);
  display.setCursor(10, 55);
  display.setTextSize(1);
  display.print("Craig Barnes");
  display.setCursor(80, 55);
  display.setTextColor(BLACK, WHITE);
  display.print(VERSION);
  display.display();
  delay(2000);
  display.fillScreen(BLACK);
  display.display();
}

void renderCurrentPatchPage() {
  display.clearDisplay();
  display.setFont(&FreeSans12pt7b);
  display.setCursor(5, 20);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println(currentPgmNum);
  display.setTextColor(WHITE);
  display.drawFastHLine(5, 31, display.width() -10, WHITE);
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(WHITE);
  display.setCursor(5, 58);
  display.setTextColor(WHITE);
  display.println(currentPatchName);
}

void renderCurrentParameterPage()
{
  switch (state)
  {
    case PARAMETER:
      display.clearDisplay();
      display.setFont(&FreeSans9pt7b);
      display.setCursor(5, 20);
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.println(currentParameter);
      display.drawFastHLine(5, 31, display.width() -10, WHITE);
      display.setCursor(5, 58);
      display.setTextColor(WHITE);
      display.println(currentValue);
      break;
  }
}

void renderDeletePatchPage() {
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setCursor(5, 20);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Delete?");
  display.drawFastHLine(5, 31, display.width() - 10, WHITE);
  display.setFont(&FreeSans9pt7b);
  display.fillRect(0, 41, display.width(), 23, WHITE);
  display.setCursor(5, 58);
  display.setTextColor(BLACK);
  display.println(patches.first().patchNo);
  display.setCursor(30, 58);
  display.setTextColor(BLACK);
  display.println(patches.first().patchName);
}

void renderDeleteMessagePage() {
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setTextSize(1);
  display.setCursor(5, 10);
  display.setTextColor(WHITE);
  display.println("Renumbering");
  display.setCursor(5, 58);
  display.println("SD Card");
}

void renderSavePage() {
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setCursor(5, 20);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Save?");
  display.drawFastHLine(5, 31, display.width() - 10, WHITE);
  display.fillRect(0, 41, display.width(), 23, WHITE);
  display.setCursor(5, 58);
  display.setTextColor(BLACK);
  display.println(patches.last().patchNo);
  display.setCursor(30, 58);
  display.setTextColor(BLACK);
  display.println(patches.last().patchName);
}

void renderReinitialisePage() {
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 20);
  display.println("Initialise to");
  display.setCursor(5, 58);
  display.println("panel setting");
}

void renderPatchNamingPage() {
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 20);
  display.println("Rename Patch");
  display.drawFastHLine(5, 31, display.width() - 10, WHITE);
  display.setTextColor(WHITE);
  display.setCursor(5, 58);
  display.println(newPatchName);
}

void renderRecallPage() {
  display.clearDisplay();
  display.setCursor(5, 16);
  display.setTextColor(WHITE);
  display.println(patches.last().patchNo);
  display.setCursor(30, 16);
  display.setTextColor(WHITE);
  display.println(patches.last().patchName);

  display.fillRect(0, 22, display.width(), 23, WHITE);
  display.setCursor(5, 39);
  display.setTextColor(BLACK);
  display.println(patches.first().patchNo);
  display.setCursor(30, 39);
  display.setTextColor(BLACK);
  display.println(patches.first().patchName);

  display.setCursor(5, 62);
  display.setTextColor(WHITE);
  patches.size() > 1 ? display.println(patches[1].patchNo) : display.println(patches.last().patchNo);
  display.setCursor(30, 62);
  display.setTextColor(WHITE);
  patches.size() > 1 ? display.println(patches[1].patchName) : display.println(patches.last().patchName);
}

void showRenamingPage(String newName)
{
  newPatchName = newName;
}

void renderUpDown(uint16_t x, uint16_t y, uint16_t colour) {
  //Produces up/down indicator glyph at x,y
  display.setCursor(x, y);
  display.fillTriangle(x, y, x + 8, y - 8, x + 16, y, colour);
  display.fillTriangle(x, y + 4, x + 8, y + 12, x + 16, y + 4, colour);
}


void renderSettingsPage()
{
  display.clearDisplay();
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 20);
  display.print(currentSettingsOption);
  if (currentSettingsPart == SETTINGS) renderUpDown(100, 10, WHITE);
  display.drawFastHLine(5, 31, display.width() - 10, WHITE);
  display.setTextColor(WHITE);
  display.setCursor(5, 58);
  display.println(currentSettingsValue);
  if (currentSettingsPart == SETTINGSVALUE) renderUpDown(100, 45, WHITE);
}

void showCurrentParameterPage(const char *param, float val, int pType)
{
  currentParameter = param;
  currentValue = String(val);
  currentFloatValue = val;
  paramType = pType;
  startTimer();
}

void showCurrentParameterPage(const char *param, String val, int pType)
{
  if (state == SETTINGS || state == SETTINGSVALUE)state = PARAMETER;//Exit settings page if showing
  currentParameter = param;
  currentValue = val;
  paramType = pType;
  startTimer();
}

void showCurrentParameterPage(const char *param, String val)
{
  showCurrentParameterPage(param, val, PARAMETER);
}

void showPatchPage(String number, String patchName)
{
  currentPgmNum = number;
  currentPatchName = patchName;
}

void showSettingsPage(char *  option, char * value, int settingsPart)
{
  currentSettingsOption = option;
  currentSettingsValue = value;
  currentSettingsPart = settingsPart;
}

void updateScreen() {
  switch (state) {
    case PARAMETER:
      if ((millis() - timer) > DISPLAYTIMEOUT) {
        renderCurrentPatchPage();
      } else {
        if ((millis() - firsttimer) > 2000) {
        renderCurrentParameterPage();
        }
      }
      break;
    case RECALL:
      renderRecallPage();
      break;
    case SAVE:
      renderSavePage();
      break;
    case REINITIALISE:
      renderReinitialisePage();
      display.display();  //update before delay
      state = PARAMETER;
      break;
    case PATCHNAMING:
      renderPatchNamingPage();
      break;
    case PATCH:
      renderCurrentPatchPage();
      break;
    case DELETE:
      renderDeletePatchPage();
      break;
    case DELETEMSG:
      renderDeleteMessagePage();
      break;
    case SETTINGS:
    case SETTINGSVALUE:
      renderSettingsPage();
      break;
  }
  display.display();
}

void setupDisplay() {
  updateScreen();
}
