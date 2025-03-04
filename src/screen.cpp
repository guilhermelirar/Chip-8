#include "screen.hpp"
#include "chip8.hpp"

#include <cstring>

Screen::Screen(CHIP8* chip8): chip8(chip8) {}

void Screen::Clear() {
  memset(buffer, false, 64 * 32);
}

void Screen::drawSprite(uint8_t x, uint8_t y, 
                  uint8_t spriteHeight, 
                  uint8_t *sprite) {
 
  for (uint8_t i = 0; i < spriteHeight; i++) {
    uint8_t spriteLine = sprite[i];
    
    int destY = (y + i) % 32; // Y where the line will be draw

    bool lineBool[8];
    for (uint8_t j = 0; j < 8; j++) {
      lineBool[j] = (spriteLine >> (7 - j)) & 0x01;
    }

    for (uint8_t j = 0; j < 8; j++) {
      uint8_t destXforPixel = (x + j) % 64; // X position of the j pixel
      uint8_t idxInBuffer = destXforPixel + (destY * 64);
      
      bool newPixelValue = (buffer[idxInBuffer]^lineBool[j]);

      if (buffer[idxInBuffer] && !newPixelValue) {
        chip8->cpu.V[0xF] = 1;
      }
      buffer[idxInBuffer] = newPixelValue;
    }
  }
}

