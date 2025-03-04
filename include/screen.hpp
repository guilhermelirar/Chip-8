#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <cstdint>

class CHIP8;

class Screen {
public:
  CHIP8 *chip8;         // CHIP-8 System
  bool buffer[0x800];   // Pixel buffer

  Screen(CHIP8 *chip8); // Constructor

  void Clear();         // Clears display
  
  // Draws a sprite of certain height,
  // at coordinates x and y
  void drawSprite(uint8_t x, uint8_t y, 
                  uint8_t spriteHeight, 
                  uint8_t *sprite);
};

#endif // SCREEN_HPP
