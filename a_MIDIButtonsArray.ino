unsigned long debounceDelay = 10;
unsigned long lastDebounce = 0;

bool lastButtonState = 0;
bool pressedNoteButtonState = 1;

void playMIDINotes() {
  for (byte i = 0; i < rowsLength; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], LOW);

    for (byte j = 0; j < columnsLength; j++) {
      pressedNoteButtonState = digitalRead(columns[j]);

      // Only detect state changes.
      if (pressedNoteButtonState == noteButtons[i][j]->lastState || (millis() - lastDebounce) <= debounceDelay) {
        continue;
      }

      if (!pressedNoteButtonState) {
        noteButtons[i][j]->playNote(sensorMode, sensorValue, sensorOctaveShift);
      }

      if (pressedNoteButtonState) {
        noteButtons[i][j]->cancelNote();
      }
      noteButtons[i][j]->lastState = pressedNoteButtonState;
      lastDebounce = millis();
    }
    pinMode(rows[i], INPUT);
  }
}
