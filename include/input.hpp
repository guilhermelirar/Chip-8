#ifndef INPUT_HPP
#define INPUT_HPP

#include <cstdint>

class Input {
  public:
    static void HandleInput();
    static bool IsKeyDown(uint8_t key);

#ifdef UNIT_TEST
    static bool* GetKeyStateForTest() { return keyState; }
#endif

  private:
    static bool keyState[16];
};

#endif // INPUT_HPP

