//void cancelAllNotes() {
//  byte buttonsCounter = 0;
//  for (byte i = 0; i < rowsLength; i++) {
//    for (byte j = 0; j < columnsLength; j++) {
//      noteButtons[i][j]->cancelNote();
//      buttonsCounter++;
//    }
//  }
//}
//

void handleSustain() {
  byte sustainValue =  pressedNavButtonStates[0] ? 64 : 0;
  midiEventPacket_t sutainPacket = {0x0B, 0xB0, 64, sustainValue};
  MidiUSB.sendMIDI(sutainPacket);
  MidiUSB.flush();
}

byte newOctave;

void handleUnderButtonModes(bool pressedButtonState) {
  switch (underButtonMode) {
    case 0:
      newOctave = !pressedButtonState ? currentStartingOctave + 1 : currentStartingOctave;
      assignNotesToButtons(currentStartingNote, newOctave, scales[currentScale], scaleLengths[currentScale]);
      break;
    case 1:
      newOctave = !pressedButtonState ? currentStartingOctave - 1 : currentStartingOctave;
      assignNotesToButtons(currentStartingNote, newOctave, scales[currentScale], scaleLengths[currentScale]);
      break;
    case 2:
      handleSustain();
      break;
  }
}
