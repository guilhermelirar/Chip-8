#include "screen.hpp"
#include "chip8.hpp"

#include <cstring>

Screen::Screen(CHIP8* chip8): chip8(chip8) {}

void Screen::Clear() {
  memset(buffer, false, 64 * 32);
}
