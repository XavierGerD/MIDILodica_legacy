
boolean lastButtonState1 = HIGH;
boolean newButtonState1 = HIGH;

boolean lastButtonState2 = HIGH;
boolean newButtonState2 = HIGH;

boolean lastButtonState3 = HIGH;
boolean newButtonState3 = HIGH;

const byte menuDebounceDelay = 5;
unsigned long lastMenuDebounce = 0;

void ManageNavigationButtons() {
  for (byte i = 0; i < rowsLength; i++) {
    bool pressedButtonState = digitalRead(rows[i]);
    if ((millis() - lastMenuDebounce) > menuDebounceDelay) {

    }
    //    switch(i){
    //      case 1:
    //
    //    }
  }
  newButtonState1 = digitalRead(menuDown);
  if ((millis() - lastMenuDebounce) > menuDebounceDelay && newButtonState1 != lastButtonState1) {
    if (newButtonState1) {
      lastMenuDebounce = millis();
      handleNavigatorDown();
    }

    lastButtonState1 = newButtonState1;
  }

  newButtonState2 = digitalRead(menuSelect);
  if ((millis() - lastMenuDebounce) > menuDebounceDelay && newButtonState2 != lastButtonState2) {
    if (newButtonState2) {
      lastMenuDebounce = millis();
      handleNavigationSelect();
    }

    lastButtonState2 = newButtonState2;
  }


  newButtonState3 = digitalRead(menuUp);
  if ((millis() - lastMenuDebounce) > menuDebounceDelay && newButtonState3 != lastButtonState3) {
    if (newButtonState3) {
      lastMenuDebounce = millis();
      handleNavigatorUp();
    }
    lastButtonState3 = newButtonState3;
  }
}

void handleNavigationSelect() {
  Menu currentItem = allMenus[currentMenu][currentMenuItem];
  currentItem.onAction(currentItem.submenuTarget);
  drawMenu();
}

void handleNavigatorDown() {
  currentMenuItem += 1;
  if (currentMenuItem == menuLengths[currentMenu] - 1) {
    currentMenuItem = 0;
  }
  drawMenu();
}

void handleNavigatorUp() {
  currentMenuItem -= 1;
  if (currentMenuItem < 0) {
    currentMenuItem = menuLengths[currentMenu] - 1;
  }
  drawMenu();
}


void onPressUp() {
  allNumberSelectMenus[currentNumberSelectMenu].onPressUp();
}

void onPressDown() {
  allNumberSelectMenus[currentNumberSelectMenu].onPressDown();
}

void onConfirm() {
  allNumberSelectMenus[currentNumberSelectMenu].onConfirm();
}
void onCancel() {
    allNumberSelectMenus[currentNumberSelectMenu].onCancel();
}
