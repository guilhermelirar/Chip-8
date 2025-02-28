#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "chip8.hpp"
#include "cpu.hpp"

static CHIP8 c;

TEST_CASE("Program counter starts at 0x200", "[CPU]") {
  REQUIRE(c.cpu.pc == 0x200);
}

TEST_CASE("Opcode 0x00E0 (CLS) clears display)", "[CPU and Screen]") {
  memset(c.screen.buffer, true, sizeof(c.screen.buffer)); // Forcing all pixels to true
  c.cpu.DecodeAndExecute(0x00E0);                         // Calls instruction
  
  // Checks if instruction worked
  for (int i = 0; i < 64 * 32; i++) { 
    REQUIRE_FALSE(c.screen.buffer[i]); 
  }
}
