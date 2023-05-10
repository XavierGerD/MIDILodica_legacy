
boolean lastUpButtonState = LOW;
boolean lastDownButtonState = LOW;
boolean lastSelectButtonState = LOW;
boolean lastCancelButtonState = LOW;
boolean lastUnderButtonState = LOW;
boolean newUnderButtonState = HIGH;

void handleButton(byte index, bool pressedButtonState) {

  switch (index) {
    //    case 0:
    //      // Not navigation technically, but it's simpler to include it
    //      // in this loop. We want to call this function both on button
    //      // press and release, as opposed to other buttons which
    //      // are only actiaved on press.
    //      if (pressedButtonState != lastUnderButtonState) {
    //        handleUnderButtonModes();
    //      }
    //      lastUnderButtonState = pressedButtonState;
    //      break;
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
      if (!pressedButtonState && !lastDownButtonState) {
        handleNavigatorDown();
      }
      lastDownButtonState = pressedButtonState;
      break;
  }
}

void handleNavigationSelect() {
  if (currentNumberSelectMenu > 0) {
    allNumberSelectMenus[currentNumberSelectMenu].onConfirm();
    return;
  }

  allMenus[currentMenu][currentMenuItem].onAction(allMenus[currentMenu][currentMenuItem].submenuTarget);
  drawMenu();
}

void handleNavigationCancel() {
  if (currentNumberSelectMenu >= 0) {
    allNumberSelectMenus[currentNumberSelectMenu].onCancel();
    return;
  }
  
  if (currentMenu == 0) {
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
  if (currentMenuItem == menuLengths[currentMenu]) {
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
