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
        pc = stack[--sp] + 2;
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

    // 0x4XNN SNE Vx, NN. Skip if Vx not equal NN
    case (0x4): {
      uint8_t regIdx = (opcode & 0x0F00) >> 8;
      bool flag = (opcode & 0x00FF) != V[regIdx];
      if (flag) pc += 2;
      break;
    }

    // 0x5XY0 SE Vx, Vy. Skip if Vx equal Vy
    case (0x5): {
      uint8_t regX = (opcode & 0x0F00) >> 8;
      uint8_t regY = (opcode & 0x00F0) >> 4;
      bool flag = V[regX] == V[regY];
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


    // 0x8.... Logic arithmetic
    case (0x8): {
      ExecuteLogicArithmetic(opcode);
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

void Interpreter::ExecuteLogicArithmetic(uint16_t opcode) {
  uint8_t mode = 0xF & opcode;
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;

  switch (mode) {
    // LD Vx, Vy
    case (0): {
      V[x] = V[y];
      break;
    }

    // OR Vx, Vy
    case (1): {
      V[x] |= V[y];
      break;
    }

    // AND Vx, Vy
    case (2): {
      V[x] &= V[y];
      break;
    }

    // AND Vx, Vy
    case (3): {
      V[x] ^= V[y];
      break;
    }

    // ADD Vx, Vy
    case (4): {
      uint16_t res = V[x] + V[y];
      V[0xF] = (res > 0xFF) ? 1 : 0;  // Set VF flag for overflow
      V[x] = res & 0xFF;              // Store the lower 8 bits of the result
      break;
    }

    // SUB Vx, Vy
    case (5): {
      uint8_t res = V[x] - V[y];
      V[0xF] = (V[x] > V[y]) ? 1 : 0;  // Set VF flag for not borrow 
      V[x] = res;
      break;
    }

    // SHR Vx, {, Vy}
    case (6): {
      V[0xF] = V[x] & 1;  // Set VF flag for LSB
      V[x] >>= 1; // Shift right
      break;
    }

    // SUBN Vx, Vy
    case (7): {
      uint8_t res = V[y] - V[x];
      V[0xF] = (V[y] > V[x]) ? 1 : 0;  // Set VF flag for not borrow 
      V[x] = res;
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
