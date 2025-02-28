#ifndef CHIP8_HPP
#define CHIP8_HPP

#include "cpu.hpp"
#include "screen.hpp"
#include <cstdint>

class CHIP8 {
public:
  uint16_t memory[0x1000]; // 4kb memory
  CPU cpu;                 // System CPU
  Screen screen;

  CHIP8();                 // Constructor
  
  // ... 
};

#endif // CHIP8_HPP
