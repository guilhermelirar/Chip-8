#include "catch.hpp"
#include "chip8.hpp"

TEST_CASE("Fonts are initialized between 050-09F", "[CHIP-8]") {
  CHIP8 c;
  for (int i = 0x50; i <= 0x9F; i++) {
    REQUIRE(c.memory[i] != 0);
  }
}

TEST_CASE("Sprite drawing results in correct behavior of VF", "[SCREEN]") {
  CHIP8 c;
  c.cpu.DecodeAndExecute(0x6F00); // Sets VF to 0
  
  // 4x4 square
  uint8_t sprite[] = {0xF0, 0x90, 0x90, 0xF0};

  // Drawing sprite at 0, 0
  c.screen.drawSprite(0, 0, 4, sprite);
  REQUIRE(c.cpu.V[0xF] == 0); // No collision
  
  // Drawing sprite at same position to force collision 
  c.screen.drawSprite(0, 0, 4, sprite);
  REQUIRE(c.cpu.V[0xF] == 1);
}
