#ifndef SCREEN_HPP
#define SCREEN_HPP

class CHIP8;

class Screen {
public:
  CHIP8* chip8;         // CHIP-8 System
  bool buffer[0x800];   // Pixel buffer
 
  Screen(CHIP8* chip8); // Constructor
  // ...
};

#endif // SCREEN_HPP
