#ifndef Interpreter_HPP
#define Interpreter_HPP

#include <cstdint>
#include <random>

class CHIP8;

class Interpreter {
private:
  uint32_t lastTimerUpdate;
public:
  uint8_t V[16];      // General purpose registers
  uint16_t I;         // Index register
  
  uint8_t delayTimer; // Timers
  uint8_t soundTimer; // ...
  
  uint16_t pc;        // Program counter
  
  uint16_t stack[16]; // Stack
  uint8_t sp;         // Stack pointer

  CHIP8* chip8;       // CHIP-8 System
 
  std::random_device rd;
  std::mt19937 gen;

  Interpreter(CHIP8* chip8);  // Constructor

  void UpdateTimer();

  void DecodeAndExecute(uint16_t opcode);
  uint8_t FetchByte();
  void RunCycle();

  void ExecuteLogicArithmetic(uint16_t opcode);
  void ExecuteFxInstruction(uint8_t x, uint8_t mode);
};

#endif // Interpreter_HPP
