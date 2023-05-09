void assignNotesToButtons(byte startingNote, byte startingOctave, byte scale[], byte lengthOfScale) {
  byte scaleCounter = 0;
  byte currentNote = findStartingPoint(startingNote, startingOctave);

  for (byte i = 0; i < rowsLength; i++) {
    for (byte j = 0; j < columnsLength; j++) {
      noteButtons[i][j]->changeButtonNote(currentNote);
      currentNote += scale[scaleCounter];
      scaleCounter++;
      if (scaleCounter == lengthOfScale) {
        scaleCounter = 0;
      }
    }
  }
}

void initializeButtons(byte startingNote, byte startingOctave, byte scale[], byte lengthOfScale) {
  byte scaleCounter = 0;
  byte currentNote = findStartingPoint(startingNote, startingOctave);
  byte counter = 0;
  for (byte i = 0; i < rowsLength; i++) {
    for (byte j = 0; j < columnsLength; j++) {
      counter++;
      noteButtons[i][j] = new NoteButton(i, j, currentNote);
      currentNote += scale[scaleCounter];
      scaleCounter++;
      if (scaleCounter == lengthOfScale) {
        scaleCounter = 0;
      }
    }
  }
}


byte findStartingPoint(byte startingNote, byte startingOctave) {
  return startingNote + 12 * startingOctave;
}
