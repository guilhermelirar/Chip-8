#ifndef CHIP8_HPP
#define CHIP8_HPP

#include "cpu.hpp"
#include "screen.hpp"
#include <cstdint>

class CHIP8 {
public:
  static constexpr uint16_t MEMORY_SIZE = 0x1000;
  uint8_t memory[MEMORY_SIZE];  // 4kb memory
  CPU cpu;                      // System CPU
  Screen screen;

  CHIP8();                      // Constructor
  
  void Run();                   // Program loop
  bool ReadRom(const char* filename);
};

#endif // CHIP8_HPP
