#include <frequencyToNote.h>
#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include "NoteButton.h"
#include "Menu.h"
#include "Scales.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define numberOfButtons 35

#define sensorPin 18
#define stripPin 19
#define menuDown 21
#define menuSelect 22
#define menuUp 20

#define TFT_CS        23
#define TFT_RST       -1
#define TFT_DC        21

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 172
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define C7 7
#define C6 8
#define C5 9
#define C4 10
#define C3 11
#define C2 12
#define C1 13
// Used for menu controls and underbutton
#define C8 20

#define R5 17
#define R4 1
#define R3 4
#define R2 5
#define R1 6

#define underButton 20

#define rowsLength 5
#define columnsLength 7

#define backgroundColor 0x5151
#define textColor 0xAE7F
#define textShadowColor 0x2318
#define selectedColor 0x3C7A

byte rows[rowsLength] = { R1, R2, R3, R4, R5 };
byte columns[columnsLength] = { C1, C2, C3, C4, C5, C6, C7 };

byte startingNote = 60;

NoteButton *noteButtons[rowsLength][columnsLength];

// MIDIFlute config

// Sensor modes:
// 0 = Velocity
// 1 = Mod Wheel
// 2 = Pitch Bend
// 3 = Octave Shift
// 4 = Custom CC
byte sensorMode = 1;

// Under Button Modes:
// 0 = Octave Up
// 1 = Octave Down
// 2 = Sustain
byte underButtonMode = 0;

// Strip pot modes:
// 0 = Velocity
// 1 = Mod Wheel
// 2 = Pitch Bend
// 3 = Octave Shift
// 4 = Custom CC
byte stripSensorMode = 2;

byte currentScale = 0;
byte currentStartingNote = 0;
byte currentStartingOctave = 4;
byte currentSensitivity = 1;

int currentChannel = 0;
int sensorMIDICC = 0;
int stripMIDICC = 0;

byte settingsLength = 7;

byte sensorOctaveShift = 0;

byte sensorSensitivities[4] = {200, 175, 150, 125};
byte minimumSensitivity = 100;

uint8_t sensorValue;
uint8_t nextVal;

uint8_t stripVal;
uint8_t nextStripVal;

uint8_t velocity = 127;

byte increment = 4;
uint16_t colors[] = { textColor, 0x545d, textShadowColor };

void showSplashScreen() {
  for (int i = 2; i >= 0; i--) {
    tft.setTextColor(colors[i]);
    tft.setCursor(72 - (increment * i), 70 + (increment * i));
    tft.print("MIDI");
    tft.setCursor(25 - (increment * i), 130  + (increment * i));
    tft.print("LODICA");
  }

  delay(3000);
}

void launchScreen() {
  tft.init(SCREEN_HEIGHT, SCREEN_WIDTH);
  tft.setRotation(1);
  tft.fillScreen(backgroundColor);

  tft.setTextSize(4);
  tft.setFont(&FreeMonoBold9pt7b);

  showSplashScreen();

  tft.setTextColor(textColor);
  tft.setTextSize(2);
  drawMenu();
}

void setup() {
  Serial.begin(9600);

  for (byte i = 0; i < rowsLength; i++) {
    pinMode(rows[i], INPUT);
  }

  for (byte j = 0; j < columnsLength; j++) {
    pinMode(columns[j], INPUT_PULLUP);
    digitalWrite(columns[j], HIGH);
  }

  pinMode(C8, INPUT_PULLUP);
  digitalWrite(C8, HIGH);

  launchScreen();
  initializeButtons(currentStartingNote, currentStartingOctave, scales[currentScale], scaleLengths[currentScale]);
}

void loop() {
  playMIDINotes();
  byte constrainedSensorVal = constrain(analogRead(sensorPin), minimumSensitivity, sensorSensitivities[currentSensitivity]);

  // Avoid unwanted noise when idle.
  if (constrainedSensorVal <= 105) {
    constrainedSensorVal = 100;
  }

  sensorValue = map(constrainedSensorVal, minimumSensitivity,  sensorSensitivities[currentSensitivity], 0, 127);

  if (sensorValue != nextVal) {
    handleSensorModes(sensorValue);
    nextVal = sensorValue;
  }

  stripVal = map(analogRead(stripPin), 0, 1023, 0, 127);

  if (stripVal != nextStripVal) {
    handleStripVal(stripVal);
    nextStripVal = stripVal;
  }

  ManageNavigationButtons();
}

void handleSensorModes(byte sensorVal) {
  switch (sensorMode) {
    case 0:
      velocity = sensorVal;
      break;
    case 1:
    default:
      midiEventPacket_t ccModWheel = {0x0B, 0xB0, 1, sensorVal};
      MidiUSB.sendMIDI(ccModWheel);
      break;
    case 2:
      midiEventPacket_t sensorPitchBendChange = {0x0B, 0xE0, 1, sensorVal};
      MidiUSB.sendMIDI(sensorPitchBendChange);
      break;
    case 3:
      handleSensorOctaveShift(sensorVal);
      break;
    case 4:
      midiEventPacket_t ccMessage = {0x0B, 0xB0, sensorMIDICC, sensorVal};
      MidiUSB.sendMIDI(ccMessage);
  }

  MidiUSB.flush();
}

void handleStripVal(byte stripVal) {
  switch (stripSensorMode) {
    case 0:
      velocity = stripVal;
      break;
    case 1:
    default:
      midiEventPacket_t ccChange = {0x0B, 0xB0, 1, stripVal};
      MidiUSB.sendMIDI(ccChange);
      break;
    case 2:
      midiEventPacket_t stripPitchBendChange = {0x0B, 0xE0, 1, stripVal};
      MidiUSB.sendMIDI(stripPitchBendChange);
      break;
    case 3:
      handleSensorOctaveShift(stripVal);
      break;
    case 4:
      midiEventPacket_t ccMessage = {0x0B, 0xB0, stripMIDICC, stripVal};
      MidiUSB.sendMIDI(ccMessage);
  }

  MidiUSB.flush();

}

void handleSensorOctaveShift(byte sensorVal) {
  sensorOctaveShift = map(sensorVal, minimumSensitivity, sensorSensitivities[currentSensitivity], 0, 3);

  for (byte i = 0; i < rowsLength; i++) {
    for (byte j = 0; j < columnsLength; j++) {
      if (noteButtons[i][j]->lastPlayedNote) {
        midiEventPacket_t noteOff = {0x08, 0x80, noteButtons[i][j]->lastPlayedNote, 127};
        MidiUSB.sendMIDI(noteOff);
        MidiUSB.flush();

        byte noteToSend = noteButtons[i][j]->note + 12 * sensorOctaveShift;
        midiEventPacket_t noteOn = {0x09, 0x90, noteToSend, 127};
        MidiUSB.sendMIDI(noteOn);
        MidiUSB.flush();
      }
    }

  }
}

void drawTextWithShadow(char* text, byte x, byte y) {
  byte offset = 2;
  tft.setTextColor(0x545d);
  tft.setCursor(x - offset, y + offset);
  tft.print(text);

  tft.setTextColor(textColor);
  tft.setCursor(x, y);
  tft.print(text);
}

void updateNumberSelectMenuScreen(String menuName, String value, byte selectedIndex, bool isMaximumThreeDigits) {
  tft.fillScreen(backgroundColor);
  tft.setTextSize(2);

  int menuNameLength = menuName.length() + 1;
  char menuNameCharArray[menuNameLength];
  menuName.toCharArray(menuNameCharArray, menuNameLength);

  int valueLength = value.length() + 1;
  char valueArray[valueLength];
  value.toCharArray(valueArray, valueLength);

  byte menuNameX = (tft.width() / 2) - (22 * menuName.length() / 2);
  drawTextWithShadow(menuNameCharArray, menuNameX, 50);

  tft.setTextSize(4);

  byte indexOffset = 1;
  if (isMaximumThreeDigits) {
    indexOffset = 2;
  }

  byte textX = (tft.width() / 2) - (44 * value.length() / 2);
  drawTextWithShadow(valueArray, textX, 120);

  byte underscorePositionOffset = (44 * (indexOffset - selectedIndex));
  drawTextWithShadow("_", textX + underscorePositionOffset, 120);
}
