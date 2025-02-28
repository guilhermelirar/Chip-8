#include "catch.hpp"
#include "chip8.hpp"

TEST_CASE("Fonts are initialized between 050-09F", "[CHIP-8]") {
  CHIP8 c;
  for (int i = 0x50; i <= 0x9F; i++) {
    REQUIRE(c.memory[i] != 0);
  }
}
