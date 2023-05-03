
void changeOctaveShift(byte newOctave) {
  currentStartingOctave += newOctave;
  if (currentStartingOctave > 7) {
    currentStartingOctave = 6;
  }
  if (currentStartingOctave < 0) {
    currentStartingOctave = 0;
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
