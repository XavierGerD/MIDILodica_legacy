
boolean lastUpButtonState = HIGH;
boolean lastDownButtonState = HIGH;
boolean lastSelectButtonState = HIGH;
boolean lastCancelButtonState = HIGH;
boolean lastUnderButtonState = HIGH;
boolean newUnderButtonState = LOW;

// Since the debounce delay is so short, it should be fine
// to use a single variable for all debounces.
const byte menuDebounceDelay = 5;
unsigned long lastMenuDebounce = 0;

void handleButton(byte index, bool pressedButtonState) {
  Serial.println(index);
  switch (index) {
    case 0:
      if (!pressedButtonState && !lastDownButtonState) {
        handleNavigatorDown();
      }
      lastDownButtonState = pressedButtonState;
      break;
    case 1:
      if (!pressedButtonState && !lastSelectButtonState) {
        handleNavigationSelect();
      }
      lastSelectButtonState = pressedButtonState;
      break;
    case 2:
      if (!pressedButtonState && !lastCancelButtonState) {
        handleNavigationCancel();
      }
      lastCancelButtonState = pressedButtonState;
      break;
    case 3:
      if (!pressedButtonState && !lastUpButtonState) {
        handleNavigatorUp();
      }
      lastUpButtonState = pressedButtonState;
      break;
    case 4:
      // Not navigation technically, but it's simpler to include it
      // in this loop. We want to call this function both on button
      // press and release, as opposed to other buttons which
      // are only actiaved on press.
      if (!pressedButtonState != lastUnderButtonState) {
        handleUnderButtonModes();
      }
      lastUnderButtonState = pressedButtonState;
      break;
  }
}

void ManageNavigationButtons() {
  for (byte i = 0; i < rowsLength; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], LOW);

    bool pressedButtonState = digitalRead(C8);
    if ((millis() - lastMenuDebounce) > menuDebounceDelay) {
      handleButton(i, pressedButtonState);
      lastMenuDebounce = millis();
    }
    pinMode(rows[i], INPUT);
  }
}

void handleNavigationSelect() {
  if (currentNumberSelectMenu > 0) {
    allNumberSelectMenus[currentNumberSelectMenu].onConfirm();
    return;
  }

  Menu currentItem = allMenus[currentMenu][currentMenuItem];
  currentItem.onAction(currentItem.submenuTarget);
  drawMenu();
}

void handleNavigationCancel() {
  Serial.println("Menu cancel?");
  if (currentNumberSelectMenu > 0) {
    allNumberSelectMenus[currentNumberSelectMenu].onCancel();
    return;
  }

  endMenuInteraction();
  drawMenu();
}

void handleNavigatorDown() {
  Serial.println("Menu down?");
  if (currentNumberSelectMenu > 0) {
    allNumberSelectMenus[currentNumberSelectMenu].onPressDown();
    return;
  }

  currentMenuItem += 1;
  if (currentMenuItem == menuLengths[currentMenu] - 1) {
    currentMenuItem = 0;
  }
  drawMenu();
}

void handleNavigatorUp() {
  Serial.println("Menu up?");
  if (currentNumberSelectMenu > 0) {
    allNumberSelectMenus[currentNumberSelectMenu].onPressUp();
    return;
  }

  currentMenuItem -= 1;
  if (currentMenuItem < 0) {
    currentMenuItem = menuLengths[currentMenu] - 1;
  }
  drawMenu();
}
