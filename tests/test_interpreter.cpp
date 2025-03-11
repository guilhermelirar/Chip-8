#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "chip8.hpp"
#include "interpreter.hpp"

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
  REQUIRE(c.interpreter.pc == secondPC+2);
  c.interpreter.DecodeAndExecute(0x00EE);
  REQUIRE(c.interpreter.sp == 0); // Stack now has 0 values
  REQUIRE(c.interpreter.pc == firstPC+2);
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
