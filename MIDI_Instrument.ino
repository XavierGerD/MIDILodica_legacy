#include <frequencyToNote.h>
#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include "MIDILodica.h"
#include "MIDIButton.h"
#include "Menu.h"
#include "StartingNote.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <SPI.h>

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

#define C1 7
#define C2 8
#define C3 9
#define C4 10
#define C5 11
#define C6 12
#define C7 13
// Used for menu controls and underbutton
#define C8 20

#define R1 17
#define R2 1
#define R3 4
#define R4 5
#define R5 6

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

const byte chromatic[] = {1};
const byte ionian[] = {2, 2, 1, 2, 2, 2, 1};
const byte dorian[] = {2, 1, 2, 2, 2, 1, 2};
const byte phrygian[] = {1, 2, 2, 2, 1, 2, 2};
const byte lydian[] = {2, 2, 2, 1, 2, 2, 1};
const byte mixolydian[] = {2, 2, 1, 2, 2, 1, 2};
const byte aeolian[] = {2, 1, 2, 2, 1, 2};
const byte locrian[] = {1, 2, 2, 2, 1, 2, 2};
const byte melodicMinor[] = {2, 1, 2, 2, 1, 3, 1};
const byte pentatonicMajor[] = {2, 2, 3, 2, 3};
const byte pentatonicMinor[] = {3, 2, 2, 3, 2};
const byte wholeTone[] = {2};
const byte octatonic21[] = {2, 1};
const byte octatonic12[] = {1, 2};

byte *scales[] = {
  chromatic,
  ionian,
  dorian,
  phrygian,
  lydian,
  mixolydian,
  aeolian,
  locrian,
  melodicMinor,
  pentatonicMajor,
  pentatonicMinor,
  wholeTone,
  octatonic12,
  octatonic21,
};

byte scaleLengths[] = {1, 7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 1, 2, 2 };

// MIDIFlute config

// Sensor modes:
// 0 = Velocity
// 1 = Mod Wheel
// 2 = Pitch Bend
// 3 = Octave Shift
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
byte stripSensorMode = 0;

byte currentScale = 0;
byte currentStartingNote = 0;
byte currentStartingOctave = 4;
byte currentSensitivity = 1;

byte currentChannel = 0;

byte settingsLength = 7;

byte sensorOctaveShift = 0;

byte sensorSensitivities[4] = {200, 175, 150, 125};
byte minimumSensitivity = 100;

byte lastUnderButtonState = HIGH;
byte nextUnderButtonState = LOW;
long lastUnderButtonDebounce;
const byte underButtonDebounceDelay = 5;

uint8_t sensorValue;
int nextVal;

uint8_t stripVal;
int nextStripVal;

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

  delay(1000);
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
    pinMode(rows[i], OUTPUT);
  }

  for (byte i = 0; i < columnsLength; i++) {
    pinMode(columns[i], INPUT_PULLUP);
  }

  pinMode(C8, INPUT_PULLUP);

  delay(1000);
  launchScreen();
  assignNotesToButtons(currentStartingNote, currentStartingOctave, scales[currentScale], scaleLengths[currentScale]);

  delay(1000);
  onPressUp();
  delay(1000);
  onPressUp();
  delay(1000);
  onPressUp();
  delay(1000);
  onConfirm();
  delay(1000);
  onPressDown();
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

  stripVal = map(analogRead(stripPin), 1023, 0, 0, 127);

  if (stripVal != nextStripVal) {
    handleStripVal(stripVal);
    nextStripVal = stripVal;
  }

  handleUnderButtonModes();
  ManageNavigationButtons();
}

void handleSensorModes(byte sensorVal) {
  switch (sensorMode) {
    case 0:
      velocity = sensorVal;
    case 1:
    default:
      midiEventPacket_t ccChange = {0x0B, 0xB0, 1, sensorVal};
      MidiUSB.sendMIDI(ccChange);
    case 2:
      midiEventPacket_t pitchBendChange = {0x0B, 0xE0, 1, sensorVal};
      MidiUSB.sendMIDI(pitchBendChange);
    case 3:
      handleSensorOctaveShift(sensorVal);
  }

  MidiUSB.flush();
}

void handleStripVal(byte stripVal) {
  switch (stripSensorMode) {
    case 0:
      velocity = stripVal;
    case 1:
    default:
      midiEventPacket_t ccChange = {0x0B, 0xB0, 1, stripVal};
      MidiUSB.sendMIDI(ccChange);
    case 2:
      midiEventPacket_t pitchBendChange = {0x0B, 0xE0, 1, stripVal};
      MidiUSB.sendMIDI(pitchBendChange);
    case 3:
      handleSensorOctaveShift(stripVal);
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

void cancelAllNotes() {
  byte buttonsCounter = 0;
  for (byte i = 0; i < rowsLength; i++) {
    for (byte j = 0; j < columnsLength; j++) {
      noteButtons[i][j]->cancelNote();
      buttonsCounter++;
    }

  }
}

void changeOctaveShift(byte newOctave) {
  currentStartingOctave += newOctave;
  if (currentStartingOctave > 7) {
    currentStartingOctave = 6;
  }
  if (currentStartingOctave < 0) {
    currentStartingOctave = 0;
  }
}

void handleUnderButtonOctaveShift(byte octaveShiftAmount) {
  changeOctaveShift(octaveShiftAmount);
  assignNotesToButtons(currentStartingNote, currentStartingOctave, scales[currentScale], scaleLengths[currentScale]);
  cancelAllNotes();
}

void handleSustain(bool newState) {
  byte sustainValue = newState ? 64 : 0;
  midiEventPacket_t sutainPacket = {0x0B, 0xB0, 64, sustainValue};
  MidiUSB.sendMIDI(sutainPacket);
  MidiUSB.flush();
}

void handleUnderButtonModes() {
  nextUnderButtonState = digitalRead(underButton);
  if ((millis() - lastUnderButtonDebounce) > underButtonDebounceDelay && nextUnderButtonState != lastUnderButtonState) {
    if (nextUnderButtonState) {
      if (underButtonMode == 0 || underButtonMode == 1) {
        byte octaveShiftAmount = underButtonMode == 0 ? 1 : -1;
        handleUnderButtonOctaveShift(octaveShiftAmount);
      }
    } else {
      if (underButtonMode == 0 || underButtonMode == 1) {
        byte octaveShiftAmount = underButtonMode == 0 ? -1 : 1;
        handleUnderButtonOctaveShift(octaveShiftAmount);
      }
    }

    if (underButtonMode == 2) {
      handleSustain(nextUnderButtonState);
    }
    lastUnderButtonState = nextUnderButtonState;
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

  drawTextWithShadow(menuNameCharArray, 10, 50);

  tft.setTextSize(4);

  byte indexOffset = 1;
  if (isMaximumThreeDigits) {
    indexOffset = 2;
  }
  drawTextWithShadow(valueArray, 50, 120);
  byte underscorePositionOffset = (4 * 11 * (indexOffset - selectedIndex));
  
  drawTextWithShadow("_", 50 + underscorePositionOffset, 120);
}
