class StartingNote {
  public:
    const char* noteName;
    byte numericValue;
    StartingNote(const char* noteName, byte numericValue) {
      this -> noteName = noteName;
      this -> numericValue = numericValue;
    }
};
