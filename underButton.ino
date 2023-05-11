void handleSustain() {
  byte sustainValue =  pressedNavButtonStates[0] ? 64 : 0;
  midiEventPacket_t sutainPacket = {0x0B, 0xB0, 64, sustainValue};
  MidiUSB.sendMIDI(sutainPacket);
  MidiUSB.flush();
}

void handleUnderButtonModes(bool pressedButtonState) {
  byte newOctave;
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
