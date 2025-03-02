#include "cpu.hpp"
#include "chip8.hpp"

CPU::CPU(CHIP8* chip8): chip8(chip8) {
  pc = 0x200;
}

void CPU::DecodeAndExecute(uint16_t opcode) {
  // Variable opcodes
  switch (opcode >> 12) {
    // Fixed opcodes
    case (0): {
      // 0x00E0 CLS Clear Screen
      if (opcode == 0x00E0)
        chip8->screen.Clear();
      break;
    }
    
    // 0x1NNN JMP to addr
    case (0x1): {
      pc = opcode & 0x0FFF;
      break;
    }

    // 0x6XNN LOAD X with NN
    case (0x6): {
      uint8_t reg = (0x0F00 & opcode) >> 8;
      uint8_t value = (0x00FF & opcode); 
      V[reg] = value;
      break;
    }

    // 0x7XNN ADDS NN to X
    case (0x7): {
      uint8_t reg = (0x0F00 & opcode) >> 8;
      uint8_t value = (0x00FF & opcode);
      V[reg] += value;
      break;
    }

    // 0xANNN LOAD I, addr
    case (0xA): {
      uint16_t newIdx = 0x0FFF & opcode;
      I = newIdx;
      break;
    }
  }  
}
