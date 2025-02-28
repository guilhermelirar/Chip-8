#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "chip8.hpp"
#include "cpu.hpp"

TEST_CASE("Program counter starts at 0x200", "[CPU]") {
  CHIP8 c;
  REQUIRE(c.cpu.pc == 0x200);
}
