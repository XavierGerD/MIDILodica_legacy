#include <frequencyToNote.h>
#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include "MIDILodica.h"
#include "MIDIButton.h"
#include "Menu.h"
#include "Scales.h"
#include "StartingNote.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_SSD1306.h>
#include <SPI.h>

#define numberOfButtons 35

#define sensorPin 18
#define stripPin 19
#define menuDown 21
#define menuSelect 22
#define menuUp 23

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

#define R1 7
#define R2 8
#define R3 9
#define R4 10
#define R5 11
#define R6 12
#define R7 13

#define C1 17
#define C2 1
#define C3 4
#define C4 5
#define C5 6

#define underButton 20

#define rowsLength 7
#define columnsLength 5

byte rows[rowsLength] = { R1, R2, R3, R4, R5, R6, R7 };
byte columns[columnsLength] = { C1, C2, C3, C4, C5 };

byte startingNote = 60;

NoteButton *noteButtons[rowsLength][columnsLength];

byte chromatic[] = {1};
byte ionian[] = {2, 2, 1, 2, 2, 2, 1};
byte dorian[] = {2, 1, 2, 2, 2, 1, 2};
byte phrygian[] = {1, 2, 2, 2, 1, 2, 2};
byte lydian[] = {2, 2, 2, 1, 2, 2, 1};
byte mixolydian[] = {2, 2, 1, 2, 2, 1, 2};
byte aeolian[] = {2, 1, 2, 2, 1, 2};
byte locrian[] = {1, 2, 2, 2, 1, 2, 2};
byte melodicMinor[] = {2, 1, 2, 2, 1, 3, 1};
byte pentatonicMajor[] = {2, 2, 3, 2, 3};
byte pentatonicMinor[] = {3, 2, 2, 3, 2};
byte wholeTone[] = {2};
byte octatonic21[] = {2, 1};
byte octatonic12[] = {1, 2};

Scale scales[14] = {
  Scale("Chromatic", chromatic),
  Scale("Ionian", ionian),
  Scale("Dorian", dorian),
  Scale("Phryigian", phrygian),
  Scale("Lydian", lydian),
  Scale("Mixolydian", mixolydian),
  Scale("Aeolian", aeolian),
  Scale("Locrian", locrian),
  Scale("Melodic Minor", melodicMinor),
  Scale("Pentatonic Major", pentatonicMajor),
  Scale("Pentatonic Minor", pentatonicMinor),
  Scale("Whole Tone", wholeTone),
  Scale("Octatonic 1-2", octatonic12),
  Scale("Octatonic 2-1", octatonic21),
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
byte underButtonDebounceDelay = 5;

uint8_t sensorValue;
int nextVal;

uint8_t stripVal;
int nextStripVal;

uint8_t velocity = 127;

void launchScreen() {
  display.clearDisplay();
  display.drawBitmap(
    0,
    0,
    epd_bitmap_MIDILodica, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
  delay(3000);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  drawMenu();
  delay(500);
}

void setup() {
  Serial.begin(9600);
  pinMode(menuUp, INPUT_PULLUP);
  pinMode(menuSelect, INPUT_PULLUP);
  pinMode(menuDown, INPUT_PULLUP);
  pinMode(underButton, INPUT_PULLUP);


  for (byte i = 0; i < rowsLength; i++) {
    pinMode(rows[i], OUTPUT);
  }

  for (byte i = 0; i < columnsLength; i++) {
    pinMode(columns[i], INPUT);
  }

  while (!Serial) {}
  launchScreen();
  assignNotesToButtons(currentStartingNote, currentStartingOctave, scales[currentScale].scale, scaleLengths[currentScale]);
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
  assignNotesToButtons(currentStartingNote, currentStartingOctave, scales[currentScale].scale, scaleLengths[currentScale]);
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
