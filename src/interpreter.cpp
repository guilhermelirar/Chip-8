#include "interpreter.hpp"
#include "chip8.hpp"

Interpreter::Interpreter(CHIP8* chip8): chip8(chip8) {
  pc = 0x200;
}

void Interpreter::DecodeAndExecute(uint16_t opcode) {
  // Variable opcodes
  switch (opcode >> 12) {
    // Fixed opcodes
    case (0): {
      // 0x00E0 CLS Clear Screen
      if (opcode == 0x00E0) {
        chip8->screen.Clear();
      }

      // RET return from subroutine
      if (opcode == 0x00EE) {
        // PC is top of stack, sp decremeted
        pc = stack[--sp] + 1;
      }
      break;
    }
    
    // 0x1NNN JMP to addr
    case (0x1): {
      pc = opcode & 0x0FFF;
      break;
    }

    // 0x2NNN CALL a subroutine located in NNN
    case (0x2): {
      uint16_t subroutine = 0x0FFF & opcode;
      stack[sp] = pc; // Push
      sp++; 
      pc = subroutine;
      break;
    }
    
    // 0x3XNN SE Vx, NN. Skip if Vx equal NN
    case (0x3): {
      uint8_t regIdx = (opcode & 0x0F00) >> 8;
      bool flag = (opcode & 0x00FF) == V[regIdx];
      if (flag) pc += 2;
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

    // 0xDXYN Draw
    case (0xD): {
      V[0xF] = 0; // Reset status register
      uint8_t x = V[(opcode & 0x0F00) >> 8]; // Sprite coordinates
      uint8_t y = V[(opcode & 0x00F0) >> 4]; // ...
      uint8_t height = opcode & 0x000F; // N of bytes (lines) of sprite
      chip8->screen.drawSprite(x, y, height, &chip8->memory[I]);
      break;
    }
  }  
}

uint8_t Interpreter::FetchByte() {
  return chip8->memory[pc++];
}

void Interpreter::RunCycle() {
  if (pc == CHIP8::MEMORY_SIZE) {
    pc = 0x200;
  }

  uint16_t opcode = FetchByte() << 8 | FetchByte();
  DecodeAndExecute(opcode);
}
