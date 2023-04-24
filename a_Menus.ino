#include <EEPROM.h>

#define menusLength 10
#define settingsLength 8
#define sensorLength 4
#define stripLength 4
#define scaleLength 14
#define startingNoteLength 12
#define startingOctaveLength 9
#define sensitivityLength 4
#define underButtonLength 3
#define patchLength 10

Menu settingsMenuItems[settingsLength] = {
  Menu("Sens. Mode", 1, navigateToSubmenu),
  Menu("Scale", 2, navigateToSubmenu),
  Menu("Tonic", 3, navigateToSubmenu),
  Menu("Octave", 4, navigateToSubmenu),
  Menu("Sens.", 5, navigateToSubmenu),
  Menu("Under btn.", 6, navigateToSubmenu),
  Menu("Save", 7, navigateToSubmenu),
  Menu("Load", 8, navigateToSubmenu),
};

Menu sensorModeMenuItems[sensorLength] = {
  Menu("Vel", 0, changeSetting),
  Menu("Mod Wh.", 1, changeSetting),
  Menu("Pitch B.", 2, changeSetting),
  Menu("8va Shift", 3, changeSetting)
};

Menu stripModeMenuItems[stripLength] = {
  Menu("Vel", 0, changeSetting),
  Menu("Mod Wh.", 1, changeSetting),
  Menu("Pitch B.", 2, changeSetting),
  Menu("8va Shift", 3, changeSetting)
};

Menu scaleMenuItems[scaleLength] = {
  Menu("Chromatic", 0, changeScale),
  Menu("Ionian", 1, changeScale),
  Menu("Dorian", 2, changeScale),
  Menu("Phryigian", 3, changeScale),
  Menu("Lydian", 4, changeScale),
  Menu("Mixolydian", 5, changeScale),
  Menu("Aeolian", 6, changeScale),
  Menu("Locrian", 7, changeScale),
  Menu("Mel. Minor", 8, changeScale),
  Menu("Penta. Maj.", 9, changeScale),
  Menu("Penta. Min.", 10, changeScale),
  Menu("Whole Tone", 11, changeScale),
  Menu("Octa. 2, 1", 12, changeScale),
  Menu("Octa. 1, 2", 13, changeScale),
};

Menu startingNoteMenuItems[startingNoteLength] = {
  Menu("C", 0, changeStartingNote),
  Menu("Db", 1, changeStartingNote),
  Menu("D", 2, changeStartingNote),
  Menu("Eb", 3, changeStartingNote),
  Menu("E", 4, changeStartingNote),
  Menu("F", 5, changeStartingNote),
  Menu("Gb", 6, changeStartingNote),
  Menu("G", 7, changeStartingNote),
  Menu("Ab", 8, changeStartingNote),
  Menu("A", 9, changeStartingNote),
  Menu("Bb", 10, changeStartingNote),
  Menu("B", 11, changeStartingNote),
};

Menu startingOctaveMenuItems[startingOctaveLength] = {
  Menu("0", 0, changeStartingOctave),
  Menu("1", 1, changeStartingOctave),
  Menu("2", 2, changeStartingOctave),
  Menu("3", 3, changeStartingOctave),
  Menu("4", 4, changeStartingOctave),
  Menu("5", 5, changeStartingOctave),
  Menu("6", 6, changeStartingOctave),
  Menu("7", 7, changeStartingOctave),
  Menu("8", 8, changeStartingOctave),
};

Menu sensitivityMenuItems[sensitivityLength] = {
  Menu("Low", 0, changeSensitivity),
  Menu("Medium", 1, changeSensitivity),
  Menu("High", 2, changeSensitivity),
  Menu("Very High", 3, changeSensitivity),
};

Menu underButtonMenuItems[underButtonLength] = {
  Menu("8va Up", 0, changeUnderButtonMode),
  Menu("8va Down", 1, changeUnderButtonMode),
  Menu("Sustain", 2, changeUnderButtonMode),
};

Menu savePatchMenuItems[patchLength] = {
  Menu("1", 0, savePatch),
  Menu("2", 1, savePatch),
  Menu("3", 2, savePatch),
  Menu("4", 3, savePatch),
  Menu("5", 4, savePatch),
  Menu("6", 5, savePatch),
  Menu("7", 6, savePatch),
  Menu("8", 7, savePatch),
  Menu("9", 8, savePatch),
  Menu("10", 9, savePatch),
};

Menu loadPatchMenuItems[patchLength] = {
  Menu("1", 0, loadPatch),
  Menu("2", 1, loadPatch),
  Menu("3", 2, loadPatch),
  Menu("4", 3, loadPatch),
  Menu("5", 4, loadPatch),
  Menu("6", 5, loadPatch),
  Menu("7", 6, loadPatch),
  Menu("8", 7, loadPatch),
  Menu("9", 8, loadPatch),
  Menu("10", 9, loadPatch),
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

byte currentMenu = 0;
byte currentMenuItem = 0;

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

byte arraySize = menuLengths[currentMenu];

byte menuTextSize = 3;
byte menuItemOffset = 45;

void navigateToSubmenu(byte target) {
  currentMenu = target;
  currentMenuItem = 0;
}

void changeSetting(byte setting) {
  sensorMode = setting;
  navigateToSubmenu(0);
}

void changeStripSetting(byte setting) {
  stripSensorMode = setting;
  navigateToSubmenu(0);
}


void endMenuInteraction() {
  assignNotesToButtons(currentStartingNote, currentStartingOctave, scales[currentScale].scale, scaleLengths[currentScale]);
  navigateToSubmenu(0);
}

void changeScale(byte note) {
  currentScale = currentMenuItem;
  endMenuInteraction();
}

void changeStartingNote(byte note) {
  currentStartingNote = note;
  endMenuInteraction();
}

void changeStartingOctave(byte octave) {
  currentStartingOctave = octave;
  endMenuInteraction();
}

void changeSensitivity(byte setting) {
  currentSensitivity = setting;
  endMenuInteraction();
}

void changeUnderButtonMode(byte newValue) {
  underButtonMode = newValue;
  endMenuInteraction();
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

void drawMenu() {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(allMenus[currentMenu][currentMenuItem].menuName);
  display.display();
}

boolean lastButtonState1 = HIGH;
boolean newButtonState1 = HIGH;

boolean lastButtonState2 = HIGH;
boolean newButtonState2 = HIGH;

boolean lastButtonState3 = HIGH;
boolean newButtonState3 = HIGH;

unsigned long menuDebounceDelay = 5;
unsigned long lastMenuDebounce = 0;

void ManageNavigationButtons() {
  newButtonState1 = digitalRead(menuDown);
  if ((millis() - lastMenuDebounce) > menuDebounceDelay && newButtonState1 != lastButtonState1) {
    if (newButtonState1) {
      lastMenuDebounce = millis();
      handleNavigatorDown();
    }

    lastButtonState1 = newButtonState1;
  }

  newButtonState2 = digitalRead(menuSelect);
  if ((millis() - lastMenuDebounce) > menuDebounceDelay && newButtonState2 != lastButtonState2) {
    if (newButtonState2) {
      lastMenuDebounce = millis();
      handleNavigationSelect();
    }

    lastButtonState2 = newButtonState2;
  }


  newButtonState3 = digitalRead(menuUp);
  if ((millis() - lastMenuDebounce) > menuDebounceDelay && newButtonState3 != lastButtonState3) {
    if (newButtonState3) {
      lastMenuDebounce = millis();
      handleNavigatorUp();
    }
    lastButtonState3 = newButtonState3;
  }
}

void handleNavigationSelect() {
  Menu currentItem = allMenus[currentMenu][currentMenuItem];
  currentItem.onAction(currentItem.submenuTarget);
  arraySize = menuLengths[currentMenu];
  drawMenu();
}

void handleNavigatorDown() {
  currentMenuItem += 1;
  if (currentMenuItem == menuLengths[currentMenu]) {
    currentMenuItem = 0;
  }
  drawMenu();
}

void handleNavigatorUp() {
  if (currentMenuItem - 1 < 0) {
    currentMenuItem = menuLengths[currentMenu] - 1;
  } else {
    currentMenuItem -= 1;
  }
  drawMenu();
}
