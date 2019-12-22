#include "chip.h"

chip::Chip8 chipPtr;

int main()
{
  chipPtr.LoadRom("res/games/PONG2");

  while(true)
  {
    chipPtr.FetchInstruction();
    chipPtr.DecodeInstruction();
  }

  return 0;
}
