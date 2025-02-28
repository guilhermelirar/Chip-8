#include "cpu.hpp"
#include "chip8.hpp"

CPU::CPU(CHIP8* chip8): chip8(chip8) {
  pc = 0x200;
}

void CPU::DecodeAndExecute(uint16_t opcode) {
  switch (opcode) {
    case (0x00E0): {
      return chip8->screen.Clear(); 
    }
  }
}
