#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "chip8.hpp"
#include "cpu.hpp"

static CHIP8 c;

TEST_CASE("Program counter starts at 0x200", "[CPU]") {
  REQUIRE(c.cpu.pc == 0x200);
}

// 00E0
TEST_CASE("Opcode 0x00E0 (CLS) clears display", "[CPU and Screen]") {
  memset(c.screen.buffer, true, sizeof(c.screen.buffer)); // Forcing all pixels to true
  c.cpu.DecodeAndExecute(0x00E0);                         // Calls instruction
  
  // Checks if instruction worked
  for (int i = 0; i < 64 * 32; i++) { 
    REQUIRE_FALSE(c.screen.buffer[i]); 
  }
}

// 1NNN
TEST_CASE("Opcode 0x1NNN (JMP) jumps to address NNN", "[CPU]") {
  c.cpu.DecodeAndExecute(0x1ABC);
  REQUIRE(c.cpu.pc == 0xABC);
}

// 6XNN
TEST_CASE("Opcode 0x6XNN loads a register to NN", "[CPU]") {
  c.cpu.DecodeAndExecute(0x6AFF);
  REQUIRE(c.cpu.V[0xA] == 0xFF);
}

// 7XNN
TEST_CASE("Opcode 0x7XNN adds NN to register X", "[CPU]") {
  c.cpu.DecodeAndExecute(0x600A); // Initializes V[0] with A
  c.cpu.DecodeAndExecute(0x7001);
  REQUIRE(c.cpu.V[0] == 0xB);     // A + 1
}

// ANNN
TEST_CASE("Opcode 0xANNN loads index (I) register to NNN", "[CPU]") {
  c.cpu.DecodeAndExecute(0xABCD); // Index register points to BCD
  REQUIRE(c.cpu.I == 0xBCD);
}

// DXYN
TEST_CASE("Opcode 0xDXYN draws sprite of N height, defined in I, at Vx, Vy",
          "[CPU]") {

  c.screen.Clear();

  c.cpu.DecodeAndExecute(0x6000); // V[0] to 0
  c.cpu.DecodeAndExecute(0xA050); // I points to "0" in system font definition
  c.cpu.DecodeAndExecute(0xD005); // Draw height 5 sprite at 0, 0
  
  REQUIRE(c.cpu.V[0xF] == 0);     // No collision yet
  c.cpu.DecodeAndExecute(0xD005); // Forcing collision
  REQUIRE(c.cpu.V[0xF] == 1);
  c.screen.Clear();
}

// 00EE and 2NNN
TEST_CASE("Opcode 2NNN jumps do subroutine (CALL), 00EE returns (RET)", "[CPU]") {
  // Subroutine
  uint16_t firstPC = c.cpu.pc;
  c.cpu.DecodeAndExecute(0x2ABC); // CALL
  REQUIRE(c.cpu.sp == 1); // Stack now has 1 value
  REQUIRE(c.cpu.stack[0] == firstPC); // Previous PC was stored in Stack
  REQUIRE(c.cpu.pc == 0xABC); // PC is now subroutine address

  // Nesting
  uint16_t secondPC = c.cpu.pc;
  c.cpu.DecodeAndExecute(0x2DEF);
  REQUIRE(c.cpu.sp == 2); // Stack now has 2 values
  REQUIRE(c.cpu.stack[1] == secondPC); // Previous PC was stored in Stack
  REQUIRE(c.cpu.pc == 0xDEF); // PC is now subroutine address
  
  // Returning
  c.cpu.DecodeAndExecute(0x00EE);
  REQUIRE(c.cpu.sp == 1); // Stack now has 1 values
  REQUIRE(c.cpu.pc == secondPC+1);
  c.cpu.DecodeAndExecute(0x00EE);
  REQUIRE(c.cpu.sp == 0); // Stack now has 0 values
  REQUIRE(c.cpu.pc == firstPC+1);
}
