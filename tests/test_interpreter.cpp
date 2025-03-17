#define CATCH_CONFIG_MAIN
#define UNIT_TEST
#include "catch.hpp"
#include "chip8.hpp"
#include "interpreter.hpp"
#include "input.hpp"

static CHIP8 c;

TEST_CASE("Program counter starts at 0x200", "[Interpreter]") {
  REQUIRE(c.interpreter.pc == 0x200);
}

// 00E0
TEST_CASE("Opcode 0x00E0 (CLS) clears display", "[Interpreter and Screen]") {
  memset(c.screen.buffer, true, sizeof(c.screen.buffer)); // Forcing all pixels to true
  c.interpreter.DecodeAndExecute(0x00E0);                 // Calls instruction
  
  // Checks if instruction worked
  for (int i = 0; i < 64 * 32; i++) { 
    REQUIRE_FALSE(c.screen.buffer[i]); 
  }
}

// 1NNN
TEST_CASE("Opcode 0x1NNN (JMP) jumps to address NNN", "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x1ABC);
  REQUIRE(c.interpreter.pc == 0xABC);
}

// 6XNN
TEST_CASE("Opcode 0x6XNN loads a register to NN", "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x6AFF);
  REQUIRE(c.interpreter.V[0xA] == 0xFF);
}

// 7XNN
TEST_CASE("Opcode 0x7XNN adds NN to register X", "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x600A); // Initializes V[0] with A
  c.interpreter.DecodeAndExecute(0x7001);
  REQUIRE(c.interpreter.V[0] == 0xB);     // A + 1
}

// ANNN
TEST_CASE("Opcode 0xANNN loads index (I) register to NNN", "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0xABCD); // Index register points to BCD
  REQUIRE(c.interpreter.I == 0xBCD);
}

// DXYN
TEST_CASE("Opcode 0xDXYN draws sprite of N height, defined in I, at Vx, Vy",
          "[Interpreter]") {

  c.screen.Clear();

  c.interpreter.DecodeAndExecute(0x6000); // V[0] to 0
  c.interpreter.DecodeAndExecute(0xA050); // I points to "0" in system font definition
  c.interpreter.DecodeAndExecute(0xD005); // Draw height 5 sprite at 0, 0
  
  REQUIRE(c.interpreter.V[0xF] == 0);     // No collision yet
  c.interpreter.DecodeAndExecute(0xD005); // Forcing collision
  REQUIRE(c.interpreter.V[0xF] == 1);
  c.screen.Clear();
}

// 00EE and 2NNN
TEST_CASE("Opcode 2NNN jumps do subroutine (CALL), 00EE returns (RET)", "[Interpreter]") {
  // Subroutine
  uint16_t firstPC = c.interpreter.pc;
  c.interpreter.DecodeAndExecute(0x2ABC); // CALL
  REQUIRE(c.interpreter.sp == 1); // Stack now has 1 value
  REQUIRE(c.interpreter.stack[0] == firstPC); // Previous PC was stored in Stack
  REQUIRE(c.interpreter.pc == 0xABC); // PC is now subroutine address

  // Nesting
  uint16_t secondPC = c.interpreter.pc;
  c.interpreter.DecodeAndExecute(0x2DEF);
  REQUIRE(c.interpreter.sp == 2); // Stack now has 2 values
  REQUIRE(c.interpreter.stack[1] == secondPC); // Previous PC was stored in Stack
  REQUIRE(c.interpreter.pc == 0xDEF); // PC is now subroutine address
  
  // Returning
  c.interpreter.DecodeAndExecute(0x00EE);
  REQUIRE(c.interpreter.sp == 1); // Stack now has 1 values
  REQUIRE(c.interpreter.pc == secondPC);
  c.interpreter.DecodeAndExecute(0x00EE);
  REQUIRE(c.interpreter.sp == 0); // Stack now has 0 values
  REQUIRE(c.interpreter.pc == firstPC);
}

// 3XNN
TEST_CASE("Opcode 0x3XNN skips next opcode if Vx is equal to byte (SE Vx byte)",
          "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x6011);
  uint16_t pc1 = c.interpreter.pc;
  c.interpreter.DecodeAndExecute(0x3026); // Skips if V[0] == 26
  REQUIRE(c.interpreter.pc == pc1);       // PC is not incremented
  c.interpreter.DecodeAndExecute(0x6126);
  c.interpreter.DecodeAndExecute(0x3126); // V[1] == 26, so skip is done
  REQUIRE(c.interpreter.pc == pc1 + 2);   // Skip
}

// 4XNN
TEST_CASE(
    "Opcode 0x4XNN skips next opcode if Vx is not equal to byte (SNE Vx byte)",
    "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x6011);
  uint16_t pc1 = c.interpreter.pc;
  c.interpreter.DecodeAndExecute(0x4126); // V[1] == 26, so skip is not done
  REQUIRE(c.interpreter.pc == pc1);       // PC is not incremented
  c.interpreter.DecodeAndExecute(0x6126);
  c.interpreter.DecodeAndExecute(0x4026); // Skips if V[0] != 26
  REQUIRE(c.interpreter.pc == pc1 + 2);   // Skip
}

// 5XY0
TEST_CASE(
    "Opcode 0x5XY0 skips next opcode if Vx is equal to Vy (SE Vx Vy)",
    "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x6011);
  c.interpreter.DecodeAndExecute(0x6126);
  uint16_t pc1 = c.interpreter.pc;
  c.interpreter.DecodeAndExecute(0x5010); // 11 != 26, so skip is not done
  REQUIRE(c.interpreter.pc == pc1);       // PC is not incremented
  c.interpreter.DecodeAndExecute(0x6026);
  c.interpreter.DecodeAndExecute(0x5010); // Skip is done
  REQUIRE(c.interpreter.pc == pc1 + 2);
}

TEST_CASE(
    "Opcode 0x8XY0 Stores the value of register Vy in register Vx. (LD Vx, Vy)",
    "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x60AA); // V0 <- AA
  c.interpreter.DecodeAndExecute(0x8100); // V1 <- V0
  REQUIRE(c.interpreter.V[0] == 0xAA);
}

TEST_CASE(
    "Opcode 0x8XY1 Sets Vx = Vx OR Vy (OR Vx, Vy)",
    "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x60F0); // V0 <- F0
  c.interpreter.DecodeAndExecute(0x610F); // V1 <- 0F
  c.interpreter.DecodeAndExecute(0x8011); // V1 <- F0 | 0F = FF
  REQUIRE(c.interpreter.V[0] == 0xFF);
}

TEST_CASE(
    "Opcode 0x8XY2 Sets Vx = Vx & Vy (AND Vx, Vy)",
    "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x60F0); // V0 <- F0
  c.interpreter.DecodeAndExecute(0x610F); // V1 <- 0F
  c.interpreter.DecodeAndExecute(0x8012); // V1 <- F0 & 0F = 0
  REQUIRE(c.interpreter.V[0] == 0);
}

TEST_CASE(
    "Opcode 0x8XY3 Sets Vx = Vx ^ Vy (XOR Vx, Vy)",
    "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x60FF); // V0 <- FF
  c.interpreter.DecodeAndExecute(0x610F); // V1 <- 0F
  c.interpreter.DecodeAndExecute(0x8013); // V1 <- FF ^ 0F = F0
  REQUIRE(c.interpreter.V[0] == 0xF0);
}

TEST_CASE(
    "Opcode 0x8XY4 adds Vy to Vx (ADD Vx, Vy)",
    "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x6F00); // VF <- 0
  c.interpreter.DecodeAndExecute(0x60F0); // V0 <- F0
  c.interpreter.DecodeAndExecute(0x610F); // V1 <- 0F
  c.interpreter.DecodeAndExecute(0x8014); // V1 <- F0 + 0F = FF
  REQUIRE(c.interpreter.V[0] == 0xFF);
  REQUIRE(c.interpreter.V[0xF] == 0); // No overflow

  c.interpreter.DecodeAndExecute(0x6101); // V1 <- 01
  c.interpreter.DecodeAndExecute(0x8014); // V1 <- FF + 01 = (1)00
  REQUIRE(c.interpreter.V[0] == 0);
  REQUIRE(c.interpreter.V[0xF] == 1); // Overflow
}

TEST_CASE(
    "Opcode 0x8XY5 Set Vx = Vx - Vy, set VF = NOT borrow (SUB Vx, Vy)",
    "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x6F00); // VF <- 0
  c.interpreter.DecodeAndExecute(0x60FF); // V0 <- FF
  c.interpreter.DecodeAndExecute(0x610F); // V1 <- 0F
  c.interpreter.DecodeAndExecute(0x8015); // V1 <- FF - 0F = F0
  REQUIRE(c.interpreter.V[0] == 0xF0);
  REQUIRE(c.interpreter.V[0xF] == 1); // not borrow
  
  c.interpreter.DecodeAndExecute(0x61FF); // V1 <- FF
  c.interpreter.DecodeAndExecute(0x8015); // V1 <- 0F - FF
  REQUIRE(c.interpreter.V[0xF] == 0); // borrow
}

TEST_CASE(
    "Opcode 0x8XY6 Set Vx = Vx SHR 1",
    "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x6F00); // VF <- 0
  c.interpreter.DecodeAndExecute(0x6000); // V0 <- 00
  c.interpreter.DecodeAndExecute(0x8016); // Shift right
  REQUIRE(c.interpreter.V[0] == 0);
  REQUIRE(c.interpreter.V[0xF] == 0); // LSB == 0 
  
  c.interpreter.DecodeAndExecute(0x6003); // V0 <- 3
  c.interpreter.DecodeAndExecute(0x8016); // SHR
  REQUIRE(c.interpreter.V[0] == 1);
  REQUIRE(c.interpreter.V[0xF] == 1); // LSB == 1
}

TEST_CASE("Opcode 0x8XY7 Set Vx = Vy - Vx (Sub N)", "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x6F00); // VF <- 0
  c.interpreter.DecodeAndExecute(0x600F); // V0 <- 0F
  c.interpreter.DecodeAndExecute(0x61FF); // V1 <- FF
  c.interpreter.DecodeAndExecute(0x8017); // SUBN
  REQUIRE(c.interpreter.V[0] == 0xF0); // FF - 0F
  REQUIRE(c.interpreter.V[0xF] == 1); // not borrow 
}

TEST_CASE("Opcode 0x8XYE Set Vx = Vx SHL Vx", "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0x6F00); // VF <- 0
  c.interpreter.DecodeAndExecute(0x6001); // V0 <- 0000 0001 b
  c.interpreter.DecodeAndExecute(0x801E); // Shift left
  REQUIRE(c.interpreter.V[0] == 2);
  REQUIRE(c.interpreter.V[0xF] == 0); // MSB == 0 
  
  c.interpreter.DecodeAndExecute(0x6081); // V0 <- 1000 0001 b
  c.interpreter.DecodeAndExecute(0x801E); // Shift left 
  REQUIRE(c.interpreter.V[0] == 2);
  REQUIRE(c.interpreter.V[0xF] == 1); // LSB == 1
}

TEST_CASE("Opcode 0xFX65 copies values from memory into registrs",
          "[Interpreter]") {
  uint8_t v0 = 15, v1 = 26, v2 = 31, v3 = 99; // Arbitrary numbers
  c.memory[c.interpreter.I] = v0;
  c.memory[c.interpreter.I + 1] = v1;
  c.memory[c.interpreter.I + 2] = v2;
  c.memory[c.interpreter.I + 3] = v3;

  c.interpreter.DecodeAndExecute(0xF365); // Load those values
  REQUIRE(c.interpreter.V[0] == v0);
  REQUIRE(c.interpreter.V[1] == v1);
  REQUIRE(c.interpreter.V[2] == v2);
  REQUIRE(c.interpreter.V[3] == v3);
}

// FX1E
TEST_CASE("Opcode 0xFX1E increments I by Vx", "[Interpreter]") {
  c.interpreter.DecodeAndExecute(0xA001); // Index register points to 1
  c.interpreter.DecodeAndExecute(0x60FF); // V0 <- FF
  c.interpreter.DecodeAndExecute(0xF01E); // I += V0
  REQUIRE(c.interpreter.I == 0x100); // 1 + FF
}

TEST_CASE("Opcode 0xFX55 copies values from registers into memory", "[Interpreter]") {
  uint8_t v0 = 19, v1 = 20, v2 = 11, v3 = 12; // Arbitrary numbers
  c.interpreter.V[0] = v0;
  c.interpreter.V[1] = v1;
  c.interpreter.V[2] = v2;
  c.interpreter.V[3] = v3;

  c.interpreter.DecodeAndExecute(0xF355); // Load those values
  REQUIRE(c.memory[c.interpreter.I] == v0);
  REQUIRE(c.memory[c.interpreter.I + 1] == v1);
  REQUIRE(c.memory[c.interpreter.I + 2] == v2);
  REQUIRE(c.memory[c.interpreter.I + 3] == v3);
}

TEST_CASE("Opcode 0xFX33 Stores the BCD representation of Vx starting from I "
          "in memory",
          "[Interpreter]") {
  c.interpreter.V[0] = 123;
  c.interpreter.DecodeAndExecute(0xF033);
  REQUIRE(c.memory[c.interpreter.I] == 1);
  REQUIRE(c.memory[c.interpreter.I + 1] == 2);
  REQUIRE(c.memory[c.interpreter.I + 2] == 3);
}

TEST_CASE("Opcode 0x9XY0 Skips next instruction if Vx != Vy", "[Interpreter]") {
  c.interpreter.V[0] = 123;
  c.interpreter.V[1] = 123;
  uint16_t pc = c.interpreter.pc;

  c.interpreter.DecodeAndExecute(0x9010);
  REQUIRE(c.interpreter.pc == pc); // Equal then don't skip

  c.interpreter.V[1] = 124;
  c.interpreter.DecodeAndExecute(0x9010);
  REQUIRE(c.interpreter.pc == pc + 2); // Different then don't skip
}

TEST_CASE("Opcode 0xBNNN jumps to loaction NNN + V0", 
          "[Interpreter]") {
  c.interpreter.V[0] = 100;
  c.interpreter.DecodeAndExecute(0xB0FF);
  REQUIRE(c.interpreter.pc == 0xFF + 100);
}

TEST_CASE("Opcode EX9E skip next instruction if key (Vx) pressed",
          "[Interpreter]") {
  c.interpreter.V[0] = 2;
  Input::GetKeyStateForTest()[2] = true;
  uint16_t firstPC = c.interpreter.pc;
  c.interpreter.DecodeAndExecute(0xE09E);
  REQUIRE(c.interpreter.pc == firstPC + 2);
}

// EXA1
TEST_CASE("Opcode EXA1 skip next instruction if key (Vx) not presed", 
          "[Interpreter]") {
  Input::GetKeyStateForTest()[2] = true;
  uint16_t firstPC = c.interpreter.pc;
  c.interpreter.DecodeAndExecute(0xE0A1);
  REQUIRE(c.interpreter.pc == firstPC); // Key is pressed
  Input::GetKeyStateForTest()[2] = false;
  c.interpreter.DecodeAndExecute(0xE0A1);
  REQUIRE(c.interpreter.pc == firstPC + 2); // Key not pressed
}

// FX07
TEST_CASE("Opcode FX07 loads Vx with teh value of delay timer", "[Interpreter]") {
  c.interpreter.delayTimer = 99;
  c.interpreter.DecodeAndExecute(0xF107);
  REQUIRE(c.interpreter.V[1] == c.interpreter.delayTimer);
}

// FX0A
TEST_CASE("Opcode FX0A Waits for a key to be pressed, and store the value in "
          "Vx (stops all execution)",
          "[Interpreter]") {
  uint16_t firstPC = c.interpreter.pc;
  memset(Input::GetKeyStateForTest(), false, 16); // No key is pressed
  c.interpreter.DecodeAndExecute(0xF00A);
  REQUIRE(c.interpreter.pc == firstPC - 2); // Equivalent to stop the execution
  
  c.interpreter.pc += 2; // Fetch byte x 2
  Input::GetKeyStateForTest()[2] = true; // A key is pressed
  c.interpreter.DecodeAndExecute(0xF00A);
  REQUIRE(c.interpreter.pc == firstPC);
}

// FX15
TEST_CASE("Opcode FX15 loads delay timer with teh value of Vx", "[Interpreter]") {
  c.interpreter.V[3] = 100;
  c.interpreter.DecodeAndExecute(0xF315);
  REQUIRE(c.interpreter.delayTimer == c.interpreter.V[3]);
}

// FX18
TEST_CASE("Opcode FX18 loads sound timer with teh value of Vx", "[Interpreter]") {
  c.interpreter.V[4] = 100;
  c.interpreter.DecodeAndExecute(0xF418);
  REQUIRE(c.interpreter.soundTimer == c.interpreter.V[3]);
}
