#include <EEPROM.h>

#define menusLength 10
#define settingsLength 9
#define sensorLength 5
#define stripLength 4
#define scaleLength 14
#define startingNoteLength 12
#define startingOctaveLength 9
#define sensitivityLength 4
#define underButtonLength 3
#define patchLength 10

byte currentMenu = 0;
int currentNumberSelectMenu = 0;
int currentMenuItem = 0;

int getSettingsMenuSelectedValue() {
  return -1;
}

Menu settingsMenuItems[settingsLength] = {
  Menu("Sensor Mode", 1, navigateToSubmenu, getSettingsMenuSelectedValue),
  Menu("Scale", 2, navigateToSubmenu, getSettingsMenuSelectedValue),
  Menu("Tonic", 3, navigateToSubmenu, getSettingsMenuSelectedValue),
  Menu("Octave", 4, navigateToSubmenu, getSettingsMenuSelectedValue),
  Menu("Sensitivity", 5, navigateToSubmenu, getSettingsMenuSelectedValue),
  Menu("Under button", 6, navigateToSubmenu, getSettingsMenuSelectedValue),

  // submenu is 1 here because it belongs to the allNumberSelectMenus list and not the regular allMenus list.
  Menu("MIDI Channel", 1, navigateToNumberSelectSubmenu, getSettingsMenuSelectedValue),

  Menu("Save", 8, navigateToSubmenu, getSettingsMenuSelectedValue),
  Menu("Load", 9, navigateToSubmenu, getSettingsMenuSelectedValue),
};

int getSelectedSensorMode() {
  return sensorMode;
}

void changeSetting(byte setting) {
  sensorMode = setting;
  navigateToSubmenu(0);
}

Menu sensorModeMenuItems[sensorLength] =  {
  Menu("Velocity", 0, changeSetting, getSelectedSensorMode),
  Menu("Mod Wheel", 1, changeSetting, getSelectedSensorMode),
  Menu("Pitch Bend", 2, changeSetting, getSelectedSensorMode),
  Menu("Octave Shift", 3, changeSetting, getSelectedSensorMode),
  Menu("Custom CC", 1, navigateToNumberSelectSubmenu, getSelectedSensorMode),
};

int getSelectedStripMode() {
  return stripSensorMode;
}

void changeStripSetting(byte setting) {
  stripSensorMode = setting;
  navigateToSubmenu(0);
}

Menu stripModeMenuItems[stripLength] = {
  Menu("Velocity", 0, changeStripSetting, getSelectedStripMode),
  Menu("Mod Wheel", 1, changeStripSetting, getSelectedStripMode),
  Menu("Pitch Bend", 2, changeStripSetting, getSelectedStripMode),
  Menu("Octave Shift", 3, changeStripSetting, getSelectedStripMode)
};

int getSelectedScale() {
  return currentScale;
}

void changeScale(byte note) {
  currentScale = currentMenuItem;
  endMenuInteraction();
}

Menu scaleMenuItems[scaleLength] = {
  Menu("Chromatic", 0, changeScale, getSelectedScale),
  Menu("Ionian", 1, changeScale, getSelectedScale),
  Menu("Dorian", 2, changeScale, getSelectedScale),
  Menu("Phryigian", 3, changeScale, getSelectedScale),
  Menu("Lydian", 4, changeScale, getSelectedScale),
  Menu("Mixolydian", 5, changeScale, getSelectedScale),
  Menu("Aeolian", 6, changeScale, getSelectedScale),
  Menu("Locrian", 7, changeScale, getSelectedScale),
  Menu("Mel. Minor", 8, changeScale, getSelectedScale),
  Menu("Penta. Maj.", 9, changeScale, getSelectedScale),
  Menu("Penta. Min.", 10, changeScale, getSelectedScale),
  Menu("Whole Tone", 11, changeScale, getSelectedScale),
  Menu("Octatonic 2, 1", 12, changeScale, getSelectedScale),
  Menu("Octatonic 1, 2", 13, changeScale, getSelectedScale),
};

int getStartingNote() {
  return currentStartingNote;
}

void changeStartingNote(byte note) {
  currentStartingNote = note;
  endMenuInteraction();
}

Menu startingNoteMenuItems[startingNoteLength] = {
  Menu("C", 0, changeStartingNote, getStartingNote),
  Menu("Db", 1, changeStartingNote, getStartingNote),
  Menu("D", 2, changeStartingNote, getStartingNote),
  Menu("Eb", 3, changeStartingNote, getStartingNote),
  Menu("E", 4, changeStartingNote, getStartingNote),
  Menu("F", 5, changeStartingNote, getStartingNote),
  Menu("Gb", 6, changeStartingNote, getStartingNote),
  Menu("G", 7, changeStartingNote, getStartingNote),
  Menu("Ab", 8, changeStartingNote, getStartingNote),
  Menu("A", 9, changeStartingNote, getStartingNote),
  Menu("Bb", 10, changeStartingNote, getStartingNote),
  Menu("B", 11, changeStartingNote, getStartingNote),
};

int getStartingOctave() {
  return currentStartingOctave;
}

void changeStartingOctave(byte octave) {
  currentStartingOctave = octave;
  endMenuInteraction();
}

Menu startingOctaveMenuItems[startingOctaveLength] = {
  Menu("0", 0, changeStartingOctave, getStartingOctave),
  Menu("1", 1, changeStartingOctave, getStartingOctave),
  Menu("2", 2, changeStartingOctave, getStartingOctave),
  Menu("3", 3, changeStartingOctave, getStartingOctave),
  Menu("4", 4, changeStartingOctave, getStartingOctave),
  Menu("5", 5, changeStartingOctave, getStartingOctave),
  Menu("6", 6, changeStartingOctave, getStartingOctave),
  Menu("7", 7, changeStartingOctave, getStartingOctave),
  Menu("8", 8, changeStartingOctave, getStartingOctave),
};

int getSensitivity() {
  return currentSensitivity;
}

void changeSensitivity(byte setting) {
  currentSensitivity = setting;
  endMenuInteraction();
}

Menu sensitivityMenuItems[sensitivityLength] = {
  Menu("Low", 0, changeSensitivity, getSensitivity),
  Menu("Medium", 1, changeSensitivity, getSensitivity),
  Menu("High", 2, changeSensitivity, getSensitivity),
  Menu("Very High", 3, changeSensitivity, getSensitivity),
};

int getUnderButtonMode() {
  return underButtonMode;
}

void changeUnderButtonMode(byte newValue) {
  underButtonMode = newValue;
  endMenuInteraction();
}

Menu underButtonMenuItems[underButtonLength] = {
  Menu("Octave Up", 0, changeUnderButtonMode, getUnderButtonMode),
  Menu("Octave Down", 1, changeUnderButtonMode, getUnderButtonMode),
  Menu("Sustain", 2, changeUnderButtonMode, getUnderButtonMode),
};

int getPatch() {
  return -1;
}

void savePatch(byte startingLocation) {
  byte startingAddress = startingLocation * settingsLength;
  for (byte i = 0; i < settingsLength; i++) {
    byte currentAddress = i + startingAddress;
    switch (i) {
      case 0:
        EEPROM.write(currentAddress, sensorMode);
        break;
      case 1:
        EEPROM.write(currentAddress, underButtonMode);
        break;
      case 2:
        EEPROM.write(currentAddress, currentScale);
        break;
      case 3:
        EEPROM.write(currentAddress, currentStartingNote);
        break;
      case 4:
        EEPROM.write(currentAddress, currentStartingOctave);
        break;
      case 5:
        EEPROM.write(currentAddress, currentSensitivity);
        break;
      case 6:
        EEPROM.write(currentAddress, stripSensorMode);
        break;
      default: break;
    }
    endMenuInteraction();
  }
}

Menu savePatchMenuItems[patchLength] = {
  Menu("1", 0, savePatch, getPatch),
  Menu("2", 1, savePatch, getPatch),
  Menu("3", 2, savePatch, getPatch),
  Menu("4", 3, savePatch, getPatch),
  Menu("5", 4, savePatch, getPatch),
  Menu("6", 5, savePatch, getPatch),
  Menu("7", 6, savePatch, getPatch),
  Menu("8", 7, savePatch, getPatch),
  Menu("9", 8, savePatch, getPatch),
  Menu("10", 9, savePatch, getPatch),
};

void loadPatch(byte startingLocation) {
  byte startingAddress = startingLocation * settingsLength;
  for (byte i = 0; i < settingsLength; i++) {
    byte currentAddress = i + startingAddress;
    byte EEPROMVal = EEPROM.read(currentAddress);
    switch (i) {
      case 0:
        sensorMode = EEPROMVal;
        break;
      case 1:
        underButtonMode = EEPROMVal;
        break;
      case 2:
        currentScale = EEPROMVal;
        break;
      case 3:
        currentStartingNote = EEPROMVal;
        break;
      case 4:
        currentStartingOctave = EEPROMVal;
        break;
      case 5:
        currentSensitivity = EEPROMVal;
        break;
      case 6:
        stripSensorMode = EEPROMVal;
        break;
    }
  }
  endMenuInteraction();
}


Menu loadPatchMenuItems[patchLength] = {
  Menu("1", 0, loadPatch, getPatch),
  Menu("2", 1, loadPatch, getPatch),
  Menu("3", 2, loadPatch, getPatch),
  Menu("4", 3, loadPatch, getPatch),
  Menu("5", 4, loadPatch, getPatch),
  Menu("6", 5, loadPatch, getPatch),
  Menu("7", 6, loadPatch, getPatch),
  Menu("8", 7, loadPatch, getPatch),
  Menu("9", 8, loadPatch, getPatch),
  Menu("10", 9, loadPatch, getPatch),
};


Menu* allMenus[menusLength] = {
  settingsMenuItems,
  sensorModeMenuItems,
  stripModeMenuItems,
  scaleMenuItems,
  startingNoteMenuItems,
  startingOctaveMenuItems,
  sensitivityMenuItems,
  underButtonMenuItems,
  savePatchMenuItems,
  loadPatchMenuItems
};

void onUpdateMIDIChannel(int newMidiChannel) {
  currentChannel = newMidiChannel;
}

int getMidiChannel() {
  return currentChannel;
}


void onUpdateSensorMIDICC(int newMIDICC) {
  Serial.println(newMIDICC);
  sensorMIDICC = newMIDICC;
}

int getSensorMIDICC() {
  return sensorMIDICC;
}

NumberSelectMenu MIDIChannel = NumberSelectMenu("MIDI Channel", getMidiChannel, false, onUpdateMIDIChannel, updateNumberSelectMenuScreen);
NumberSelectMenu SensorMIDICC = NumberSelectMenu("MIDI CC", getSensorMIDICC, true, onUpdateSensorMIDICC, updateNumberSelectMenuScreen);

NumberSelectMenu allNumberSelectMenus[] = {
  MIDIChannel,
  SensorMIDICC
};

byte menuLengths[menusLength] = {
  settingsLength,
  sensorLength,
  stripLength,
  scaleLength,
  startingNoteLength,
  startingOctaveLength,
  sensitivityLength,
  underButtonLength,
  patchLength,
  patchLength
};

void navigateToSubmenu(byte target) {
  currentMenu = target;
  currentMenuItem = 0;
}

void navigateToNumberSelectSubmenu(byte target) {
  currentNumberSelectMenu = target;
}

void endMenuInteraction() {
  assignNotesToButtons(currentStartingNote, currentStartingOctave, scales[currentScale], scaleLengths[currentScale]);
  navigateToSubmenu(0);
}

void endNumberSelectMenuInteraction() {
  currentNumberSelectMenu = -1;
  navigateToSubmenu(0);
}
