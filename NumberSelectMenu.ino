class NumberSelectMenu {
  public:
    byte menuNameLength;
    char* menuName;

    int (*getInitialValue)();

    // Individual digits.
    // Bit of a misnomer, the first digit is the
    // rightmost digit.
    char firstDigit;
    char secondDigit;
    char thirdDigit;

    // Current digit being edited.
    int selectedDigit = 0;

    // Value to show to the user.
    char valueToShow[4];

    // Only two digits if false.
    bool isMaximumThreeDigits;

    void (*updateScreen)(char* menuName, byte menuNameLength, char* value, byte selectedIndex, bool isMaximumThreeDigit);
    void (*onMenuConfirm)(byte newValue);

    NumberSelectMenu(char* menuName, byte menuNameLength, int (*getInitialValue)(), byte isMaximumThreeDigits, void (*onMenuConfirm)(int newValue), void (*updateScreen)(char* menuName, byte menuNameLength, char* value, byte selectedIndex, bool isMaximumThreeDigit)) {
      this -> menuName = menuName;
      this -> menuNameLength = menuNameLength;
      this -> isMaximumThreeDigits = isMaximumThreeDigits;
      this -> updateScreen = updateScreen;
      this -> onMenuConfirm = onMenuConfirm;
      this -> getInitialValue = getInitialValue;
      this -> valueToShow[4] = "\0";
    }

    void updateValueToShow() {
      valueToShow[0] = thirdDigit;
      valueToShow[1] = secondDigit;
      valueToShow[2] = firstDigit;
    }

    void onLoad() {
      int initialValue = getInitialValue();
      char initVal[3];
      String valueAsString = itoa(initialValue, initVal, 10);
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

      Serial.println("Name length");
      Serial.println(menuName);
      updateScreen(menuName, menuNameLength, valueToShow, selectedDigit, isMaximumThreeDigits);
    }

    char handlePressDown (char digit) {
      char newDigit = digit + 1;
      if (newDigit < 0) {
        newDigit = 9;
      }
      return newDigit;
    }


    char handlePressUp (char digit) {
      char newDigit = digit + 1;
      if (newDigit > 9) {
        newDigit = 0;
      }
      return newDigit;
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

      updateScreen(menuName, menuNameLength, valueToShow, selectedDigit, isMaximumThreeDigits);
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

      updateScreen(menuName, menuNameLength, valueToShow, selectedDigit, isMaximumThreeDigits);
    }

    void onConfirm() {
      selectedDigit--;
      if (selectedDigit <= 0) {
        String newValueString = String(thirdDigit) + String(secondDigit) + String(firstDigit);
        onMenuConfirm(newValueString.toInt());
        endNumberSelectMenuInteraction();
        return;
      }

      updateScreen(menuName, menuNameLength, valueToShow, selectedDigit, isMaximumThreeDigits);
    }

    void onCancel() {
      selectedDigit++;
      byte maxIndex = 1;
      if (isMaximumThreeDigits) {
        maxIndex = 2;
      }

      if (selectedDigit > maxIndex) {
        endNumberSelectMenuInteraction();
        return;
      }

      updateScreen(menuName, menuNameLength, valueToShow, selectedDigit, isMaximumThreeDigits);
    }
};
