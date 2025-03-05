#ifndef Interpreter_HPP
#define Interpreter_HPP

#include <cstdint>

class CHIP8;

class Interpreter {
public:
  uint8_t V[16];      // General purpose registers
  uint16_t I;         // Index register
  
  uint8_t delayTimer; // Timers
  uint8_t soundTimer; // ...
  
  uint16_t pc;        // Program counter
  
  uint16_t stack[16]; // Stack
  uint8_t sp;         // Stack pointer

  CHIP8* chip8;       // CHIP-8 System
  
  Interpreter(CHIP8* chip8);  // Constructor
 
  void DecodeAndExecute(uint16_t opcode);
  uint8_t FetchByte();
  void RunCycle();
};

#endif // Interpreter_HPP
