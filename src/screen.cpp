#include "screen.hpp"
#include "chip8.hpp"

#include <cstring>

Screen::Screen(CHIP8* chip8): chip8(chip8) {}

void Screen::Clear() {
  memset(buffer, false, 64 * 32);
}

void Screen::drawSprite(uint8_t x, uint8_t y, 
                  uint8_t spriteHeight, 
                  uint8_t *sprite) {}

