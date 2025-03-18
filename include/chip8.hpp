#ifndef CHIP8_HPP
#define CHIP8_HPP

#include "interpreter.hpp"
#include "screen.hpp"
#include <cstdint>

class CHIP8 {
public:
  static constexpr uint8_t FONT_DATA_START = 0x50;
  static constexpr uint8_t FONT_SPRITE_HEIGHT = 5;
  static constexpr uint16_t MEMORY_SIZE = 0x1000;

  uint32_t frameStart;

  uint8_t memory[MEMORY_SIZE];  // 4kb memory
  Interpreter interpreter;      // System Interpreter
  Screen screen;

  CHIP8();                      // Constructor
  
  void Run();                   // Program loop
  bool ReadRom(const char* filename);
};

#endif // CHIP8_HPP
