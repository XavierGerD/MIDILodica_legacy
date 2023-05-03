class Menu {
  public:
    const char* menuName;
    byte submenuTarget;
    void (*onAction)(byte target);
    int (*getSelectedMenu)();
    Menu(const char* menuName, byte submenuTarget, void (*onAction)(byte target), int (*getSelectedMenu)()) {
      this -> menuName = menuName;
      this -> submenuTarget = submenuTarget;
      this -> onAction = onAction;
      this -> getSelectedMenu = getSelectedMenu;
    }
};

class NumberSelectMenu {
  public:
    const char* menuName;
    byte *initialValue;

    // Individual digits.
    // Bit of a misnomer, the first digit is the
    // rightmost digit.
    String firstDigit;
    String secondDigit;
    String thirdDigit;

    // Current digit being edited.
    byte selectedDigit = 0;

    // Value to show to the user.
    String valueToShow;

    // Only two digits if false.
    bool isMaximumThreeDigits;

    void (*updateScreen)(String menuName, String value, byte selecteIndex, byte isMaximumThreeDigits);
    void (*onMenuConfirm)(byte newValue);

    NumberSelectMenu(const char* menuName, byte *initialValue, byte isMaximumThreeDigits, void (*onMenuConfirm)(byte newValue), void (*updateScreen)(String menuName, String value, byte selectedIndex, byte isMaximumThreeDigits)) {
      this -> menuName = menuName;
      this -> isMaximumThreeDigits = isMaximumThreeDigits;
      this -> updateScreen = updateScreen;
      this -> onMenuConfirm = onMenuConfirm;
      this -> initialValue = initialValue;
    }

    void updateValueToShow() {
      valueToShow = secondDigit + firstDigit;

      if (isMaximumThreeDigits) {
        valueToShow = thirdDigit + valueToShow;
      }

    }

    void onLoad() {
      String valueAsString = String(*initialValue);

      // Easier to always work with a string of length 3
      if (valueAsString.length() < 2) {
        valueAsString = "0" + valueAsString;
      }

      if (valueAsString.length() < 3) {
        valueAsString = "0" + valueAsString;
      }

      firstDigit = valueAsString.charAt(2);
      secondDigit = valueAsString.charAt(1);
      thirdDigit = valueAsString.charAt(0);

      updateValueToShow();

      selectedDigit = 1;
      if (isMaximumThreeDigits) {
        selectedDigit = 2;
      }
      updateScreen(menuName, valueToShow, selectedDigit, isMaximumThreeDigits);
    }

    String handlePressDown (String digit) {
      int digitAsInt =  digit.toInt();
      digitAsInt--;
      if (digitAsInt < 0) {
        digitAsInt = 9;
      }
      return String(digitAsInt);
    }


    String handlePressUp (String digit) {
      int digitAsInt =  digit.toInt();
      digitAsInt++;
      if (digitAsInt > 9) {
        digitAsInt = 0;
      }
      return String(digitAsInt);
    }

    void onPressDown() {
      switch (selectedDigit) {
        case 0:
          firstDigit = handlePressDown(firstDigit);
          break;
        case 1:
          secondDigit = handlePressDown(secondDigit);
          break;
        case 2:
          thirdDigit =  handlePressDown(thirdDigit);
          break;
      }
      updateValueToShow();
      updateScreen(menuName, valueToShow, selectedDigit, isMaximumThreeDigits);
    }


    void onPressUp() {
      switch (selectedDigit) {
        case 0:
          firstDigit = handlePressUp(firstDigit);
          break;
        case 1:
          secondDigit = handlePressUp(secondDigit);
          break;
        case 2:
          thirdDigit =  handlePressUp(thirdDigit);
          break;
      }
      updateValueToShow();
      updateScreen(menuName, valueToShow, selectedDigit, isMaximumThreeDigits);
    }

    void onConfirm() {
      selectedDigit--;
      if (selectedDigit < 0) {
        String newValueString = thirdDigit + secondDigit + firstDigit;
        onMenuConfirm(newValueString.toInt());
        // endMenuInteraction?
        return;
      }

      updateScreen(menuName, valueToShow, selectedDigit, isMaximumThreeDigits);
    }

    void onCancel() {
      // endMenuInteraction?
    }
};
