
boolean lastUpButtonState = HIGH;
boolean newUpButtonState = HIGH;

boolean lastDownButtonState = HIGH;
boolean newDownButtonState = HIGH;

boolean lastSelectButtonState = HIGH;
boolean newSelectButtonState = HIGH;

boolean lastCancelButtonState = HIGH;
boolean newCancelButtonState = HIGH;

boolean lastUnderButtonState = HIGH;
boolean newUnderButtonState = LOW;

// Since the debounce delay is so short, it should be fine
// to use a single variable for all debounces.
const byte menuDebounceDelay = 5;
unsigned long lastMenuDebounce = 0;

void ManageNavigationButtons() {
  for (byte i = 0; i < rowsLength; i++) {
    bool pressedButtonState = digitalRead(rows[i]);
    if ((millis() - lastMenuDebounce) > menuDebounceDelay) {
      switch (i) {
        case 0:
          if (newDownButtonState && !lastDownButtonState) {
            handleNavigatorDown();
          }
          lastDownButtonState = newDownButtonState;
          break;
        case 1:
          if (newSelectButtonState && !lastSelectButtonState) {
            handleNavigationSelect();
          }
          lastSelectButtonState = newSelectButtonState;
          break;
        case 2:
          if (newCancelButtonState && !lastCancelButtonState) {
            handleNavigationCancel();
          }
          lastCancelButtonState = newCancelButtonState;
          break;
        case 3:
          if (newUpButtonState && !lastUpButtonState) {
            handleNavigatorUp();
          }
          lastUpButtonState = newUpButtonState;
          break;
        case 4:
          // Not navigation technically, but it's simpler to include it
          // in this loop. We want to call this function both on button
          // press and release, as opposed to other buttons which
          // are only actiaved on press.
          if (newUnderButtonState != lastUnderButtonState) {
            handleUnderButtonModes();
          }
          lastUnderButtonState = newUnderButtonState;
          break;
      }
    }
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
  if (currentNumberSelectMenu > 0) {
    allNumberSelectMenus[currentNumberSelectMenu].onCancel();
    return;
  }

  endMenuInteraction();
  drawMenu();
}

void handleNavigatorDown() {
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
