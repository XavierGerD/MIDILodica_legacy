class NumberSelectMenu {
  public:
    byte menuNameLength;
    char* menuName;

    int (*getInitialValue)();

    // Current digit being edited.
    int selectedDigit = 0;

    // Value to show to the user.
    int values[3];

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

      values[2] = valueAsString.charAt(2);
      values[1] = valueAsString.charAt(1);
      values[0] = valueAsString.charAt(0);

      selectedDigit = 1;

      if (isMaximumThreeDigits) {
        selectedDigit = 2;
      }

      char valuesToShow[4] = { values[2],  values[1],  values[0], "\0" };
      updateScreen(menuName, menuNameLength, valuesToShow, selectedDigit, isMaximumThreeDigits);
    }

    char handlePressDown (char digit) {
      int newDigit = values[selectedDigit] - 1;
      if (newDigit < 48) {
        newDigit = 57;
      }
      values[selectedDigit] = newDigit;
    }


    char handlePressUp (int selectedDigit) {
      int newDigit = values[selectedDigit] + 1;
      if (newDigit > 57) {
        newDigit = 48;
      }
      values[selectedDigit] = newDigit;
    }

    void onPressDown() {
      handlePressDown(selectedDigit);
      char valuesToShow[4] = { values[2],  values[1],  values[0], "\0" };
      updateScreen(menuName, menuNameLength, valuesToShow, selectedDigit, isMaximumThreeDigits);
    }


    void onPressUp() {
      handlePressUp(selectedDigit);
      char valuesToShow[4] = { values[2],  values[1],  values[0], "\0" };
      updateScreen(menuName, menuNameLength, valuesToShow, selectedDigit, isMaximumThreeDigits);
    }

    void onConfirm() {
      selectedDigit--;
      if (selectedDigit < 0) {
        String newValueString = String(values[0] - 48) + String(values[1] - 48) + String(values[2] - 48);
        onMenuConfirm(newValueString.toInt());
        endNumberSelectMenuInteraction();
        return;
      }
      
      char valuesToShow[4] = { values[2],  values[1],  values[0], "\0" };
      updateScreen(menuName, menuNameLength, valuesToShow, selectedDigit, isMaximumThreeDigits);
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
      char valuesToShow[4] = { values[2],  values[1],  values[0], "\0" };
      updateScreen(menuName, menuNameLength, valuesToShow, selectedDigit, isMaximumThreeDigits);
    }
};
