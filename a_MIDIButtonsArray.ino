unsigned long debounceDelay = 5;
unsigned long lastDebounce = 0;

void playMIDINotes() {
  for (byte i = 0; i < rowsLength; i++) {
    digitalWrite(rows[i], HIGH);

    for (byte j = 0; j < columnsLength; j++) {
      bool pressedButtonState = digitalRead(columns[j]);

      if ((millis() - lastDebounce) > debounceDelay) {
        if (!noteButtons[i][j]->lastState && pressedButtonState) {
          lastDebounce = millis();
          noteButtons[i][j]->playNote(sensorMode, sensorValue, sensorOctaveShift);
          noteButtons[i][j]->lastState = HIGH;
        }

        if (noteButtons[i][j]->lastState && !pressedButtonState) {
          lastDebounce = millis();
          noteButtons[i][j]->cancelNote();
          noteButtons[i][j]->lastState = LOW;
        }
      }
    }
  }
}
