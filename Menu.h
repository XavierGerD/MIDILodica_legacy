class Menu {
  public:
    const char* menuName;
    byte submenuTarget;
    void(*onAction)(byte target);
    Menu(const char* menuName, byte submenuTarget, void(*onAction)(byte target)) {
      this -> menuName = menuName;
      this -> submenuTarget = submenuTarget;
      this -> onAction = onAction;
    }
};
