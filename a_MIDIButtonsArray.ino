unsigned long debounceDelay = 5;
unsigned long lastDebounce = 0;
// For simplicity's sake, columns always use the index j and rows the index i.
// While buttons are assigned row by row, they are played column by column.
// This is why the orders of the loops here are inverted relative
// to the assignNotesToButtons function and the two-dimensional array
// noteButtons.

void playMIDINotes() {
  for (byte j = 0; j < columnsLength; j++) {
    digitalWrite(columns[j], HIGH);

    for (byte i = 0; i < rowsLength; i++) {
      bool pressedButtonState = digitalRead(rows[i]);

      if ((millis() - lastDebounce) > debounceDelay) {
        if (pressedButtonState && !noteButtons[i][j]->lastState) {
          noteButtons[i][j]->playNote(sensorMode, sensorValue, sensorOctaveShift);
        }

        if (!pressedButtonState && noteButtons[i][j]->lastState) {
          noteButtons[i][j]->cancelNote();
        }
        
        lastDebounce = millis();
        noteButtons[i][j]->lastState = pressedButtonState;
      }
    }

    digitalWrite(columns[j], LOW);
  }
}
