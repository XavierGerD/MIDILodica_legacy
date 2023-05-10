const byte debounceDelay = 10;
unsigned long lastDebounce = 0;

bool pressedButtonState = 1;

void playMIDINotes() {
  SPI.end();
  for (byte i = 0; i < rowsLength; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], LOW);

    for (byte j = 0; j < columnsLength; j++) {
      pressedButtonState = digitalRead(columns[j]);

      // Only detect state changes.
      if (pressedButtonState == noteButtons[i][j]->lastState || (millis() - lastDebounce) <= debounceDelay) {
        continue;
      }


      if (!pressedButtonState) {
        noteButtons[i][j]->playNote(sensorMode, sensorValue, sensorOctaveShift);
      }

      if (pressedButtonState) {
        noteButtons[i][j]->cancelNote();
      }


      noteButtons[i][j]->lastState = pressedButtonState;
      lastDebounce = millis();
    }

    if ((millis() - lastDebounce) > debounceDelay) {
      pressedButtonState = digitalRead(C8);
      if (!pressedButtonState) {
        handleButton(i, pressedButtonState);
        lastDebounce = millis();
      }

    }

    pinMode(rows[i], INPUT);
  }
  SPI.begin();
}
