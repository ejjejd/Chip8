#include "chip.h"

#include <pthread.h>
#include <chrono>

namespace chip
{
  void* Beep(void* arg)
  {
    audio::Audio::Beep(440.0f, 1);
    return nullptr;
  }

  unsigned char Fontset[80] =
  {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
  };

  Chip8::Chip8()
  {
    PC  = START_ADDRESS;
    SP  = 0;

    AddressI    = 0;
    Opcode      = 0;

    DelayTimer  = 0;
    SoundTimer  = 0;

    memset(Memory, 0, MEMORY_SIZE);
    memset(Registers, 0, REGISTERS_COUNT);
    memset(Stack, 0, STACK_SIZE);

    memset(Screen, 0, ORIGINAL_SCREEN_X * ORIGINAL_SCREEN_Y);
    memset(Keys, 0, KEYS_COUNT);

    memcpy(Memory, Fontset, FONTSET_SIZE);

    audio::Audio::Initialize();

    srand(time(NULL));
  }

  Chip8::~Chip8()
  {
    audio::Audio::Terminate();
  }

  void Chip8::FetchInstruction()
  {
    Opcode = Memory[PC] << 8;
    Opcode |= Memory[PC + 1];
    PC += 2;
  }

  void Chip8::LoadRom(const char* filepath)
  {
    FILE* file;
    file = fopen(filepath, "rb");
    ASSERT(file != nullptr, "Failed open file!");

    fread(&Memory[START_ADDRESS], MEMORY_SIZE, 1, file);
    fclose(file);
  }

  void Chip8::DecodeInstruction()
  {
    switch(Opcode & 0xF000)
    {
      case 0x0000:
      {
        switch(Opcode & 0x00FF)
        {
          case 0x00E0: ClearScreen(); break;
          case 0x00EE: Opcode00EE();  break;
        }
      }break;

      case 0x1000: Opcode1NNN(); break;
      case 0x2000: Opcode2NNN(); break;
      case 0x3000: Opcode3XNN(); break;
      case 0x4000: Opcode4XNN(); break;
      case 0x5000: Opcode5XY0(); break;
      case 0x6000: Opcode6XNN(); break;
      case 0x7000: Opcode7XNN(); break;

      case 0x8000:
      {
        switch(Opcode & 0x000F)
        {
          case 0x0: Opcode8XY0(); break;
          case 0x1: Opcode8XY1(); break;
          case 0x2: Opcode8XY2(); break;
          case 0x3: Opcode8XY3(); break;
          case 0x4: Opcode8XY4(); break;
          case 0x5: Opcode8XY5(); break;
          case 0x6: Opcode8XY6(); break;
          case 0x7: Opcode8XY7(); break;
          case 0xE: Opcode8XYE(); break;
        }
      }break;

      case 0x9000: Opcode9XY0(); break;

      case 0xA000: OpcodeANNN(); break;
      case 0xB000: OpcodeBNNN(); break;
      case 0xC000: OpcodeCXNN(); break;
      case 0xD000: OpcodeDXYN(); break;

      case 0xE000:
      {
        switch(Opcode & 0x000F)
        {
          case 0xE: OpcodeEX9E(); break;
          case 0x1: OpcodeEXA1(); break;
        }
      }break;

      case 0xF000:
      {
        switch(Opcode & 0x00FF)
        {
          case 0x07: OpcodeFX07(); break;
          case 0x0A: OpcodeFX0A(); break;
          case 0x15: OpcodeFX15(); break;
          case 0x18: OpcodeFX18(); break;
          case 0x1E: OpcodeFX1E(); break;
          case 0x29: OpcodeFX29(); break;
          case 0x33: OpcodeFX33(); break;
          case 0x55: OpcodeFX55(); break;
          case 0x65: OpcodeFX65(); break;
        }
      }break;
    }

    if(DelayTimer > 0)
      --DelayTimer;
    if(SoundTimer > 0)
    {
        if(SoundTimer == 1)
        {
          pthread_t pt;
          pthread_create(&pt, nullptr, Beep, nullptr);
          pthread_detach(pt);
        }

        --SoundTimer;
    }

    //Intruction limit
    usleep(SLEEP_TIME);
  }


  /*  Instructions  */

  void Chip8::ClearScreen()
  {
    memset(Screen, 0, 64 * 32);
    PRINT("Clear Screen\n");
  }

  void Chip8::Opcode00EE()
  {
    --SP;
    PC = Stack[SP];

    PRINT("Return from subroutine\n");
  }

  void Chip8::Opcode1NNN()
  {
    PC = Opcode & 0x0FFF;

    PRINT("Jump\n");
  }

  void Chip8::Opcode2NNN()
  {
    Stack[SP] = PC;
    ++SP;
    PC = Opcode & 0x0FFF;

    PRINT("Call subroutine\n");
  }

  void Chip8::Opcode3XNN()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);

    if(Registers[reg] == (Opcode & 0x00FF))
      PC += 2;

    PRINT("Condition 3XNN\n");
  }

  void Chip8::Opcode4XNN()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);

    if(Registers[reg] != (Opcode & 0x00FF))
      PC += 2;

    PRINT("Condition 4XNN\n");
  }

  void Chip8::Opcode5XY0()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    if(Registers[regX] == Registers[regY])
      PC += 2;

    PRINT("Condition 5XY0\n");
  }

  void Chip8::Opcode6XNN()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);
    Registers[reg] = Opcode & 0x00FF;

    PRINT("Fill Register 6XNN \n");
  }

  void Chip8::Opcode7XNN()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);
    Registers[reg] += (Opcode & 0x00FF);

    PRINT("Update Register value 7XNN\n");
  }

  void Chip8::Opcode8XY0()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    Registers[regX] = Registers[regY];

    PRINT("Set Register value 8XY0\n");
  }

  void Chip8::Opcode8XY1()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    Registers[regX] |= Registers[regY];

    PRINT("Update Register value 8XY1\n");
  }

  void Chip8::Opcode8XY2()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    Registers[regX] &= Registers[regY];

    PRINT("Update Register value 8XY2\n");
  }

  void Chip8::Opcode8XY3()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    Registers[regX] ^= Registers[regY];

    PRINT("Update Register value 8XY3\n");
  }

  void Chip8::Opcode8XY4()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    if(Registers[regX] < Registers[regY])
      Registers[0xF] = 1;
    else
      Registers[0xF] = 0;

    Registers[regX] += Registers[regY];

    PRINT("Update Register value 8XY4\n");
  }

  void Chip8::Opcode8XY5()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    if(Registers[regX] < Registers[regY])
      Registers[0xF] = 0;
    else
      Registers[0xF] = 1;

    Registers[regX] -= Registers[regY];

    PRINT("Update Register value 8XY5\n");
  }

  void Chip8::Opcode8XY6()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);

    Registers[0xF] = Registers[regX] & 0x1;
    Registers[regX] >>= 1;

    PRINT("Update Register value 8XY6\n");
  }

  void Chip8::Opcode8XY7()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    if(Registers[regY] < Registers[regX])
      Registers[0xF] = 0;
    else
      Registers[0xF] = 1;

    Registers[regX] = (Registers[regY] - Registers[regX]);

    PRINT("Update Register value 8XY7\n");
  }

  void Chip8::Opcode8XYE()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);

    Registers[0xF] = Registers[regX] >> 7;
    Registers[regX] <<= 1;

    PRINT("Update Register value 8XYE\n");
  }

  void Chip8::Opcode9XY0()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    if(Registers[regX] != Registers[regY])
      PC += 2;

    PRINT("Condition 9XY0\n");
  }

  void Chip8::OpcodeANNN()
  {
    AddressI = (Opcode & 0x0FFF);
    PRINT("Update Address Register ANNN\n");
  }

  void Chip8::OpcodeBNNN()
  {
    PC = ((Opcode & 0x0FFF) + Registers[0x0]);

    PRINT("Jump BNNN\n");
  }

  void Chip8::OpcodeCXNN()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);
    uint8_t value = Opcode & 0x00FF;

    uint8_t randValue = ((rand() % 255) + 1);

    Registers[reg] = value & randValue;

    PRINT("Generate Random Value\n")
  }

  void Chip8::OpcodeDXYN()
  {
    uint8_t x		= Registers[((Opcode & 0x0F00) >> 8)];
    uint8_t y		= Registers[((Opcode & 0x00F0) >> 4)];
    uint8_t height	= Opcode & 0x000F;
    uint8_t pixel;

    Registers[0xF] = 0;
    for (int yLine = 0; yLine < height; ++yLine)
    {
      pixel = Memory[AddressI + yLine];
      for (int xLine = 0; xLine < 8; ++xLine)
      {
        if ((pixel & (0x80 >> xLine)) != 0)
        {
          if (Screen[x + xLine][y + yLine] == 1)
            Registers[0xF] = 1;
          Screen[x + xLine][y + yLine] ^= 1;
        }
      }
    }

    PRINT("Draw Sprite\n");
  }

  void Chip8::OpcodeEX9E()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);

    if(Keys[Registers[reg]] != 0)
      PC += 2;

    PRINT("Condition EX9E\n");
  }

  void Chip8::OpcodeEXA1()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);

    if(Keys[Registers[reg]] == 0)
      PC += 2;

    PRINT("Condition EXA1\n");
  }

  void Chip8::OpcodeFX07()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);
    Registers[reg] = DelayTimer;

    PRINT("Timer Opcode FX07\n");
  }

  void Chip8::OpcodeFX0A()
  {
    int key = -1;

    for(int i = 0; i < 16; ++i)
    {
      if(Keys[i] != 0)
        key = i;
    }

    if(key != -1)
    {
      uint8_t reg = ((Opcode & 0x0F00) >> 8);
      Registers[reg] = key;
    }
    else
      PC -= 2;

    PRINT("Awaited key press...\n");
  }

  void Chip8::OpcodeFX15()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);
    DelayTimer = Registers[reg];

    PRINT("Timer Opcode FX15");
  }

  void Chip8::OpcodeFX18()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);
    SoundTimer = Registers[reg];

    PRINT("Updata Sound Timer\n");
  }

  void Chip8::OpcodeFX1E()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);

    if((Registers[reg] + AddressI) > 0xFFF)
      Registers[0xF] = 1;
    else
      Registers[0xF] = 0;

    AddressI += Registers[reg];

    PRINT("Updata Register I value FX1E\n");
  }

  void Chip8::OpcodeFX29()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);
    AddressI = Registers[reg] * 0x5;

    PRINT("Update Register I value FX29\n");
  }

  void Chip8::OpcodeFX33()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);

    Memory[AddressI]		  = Registers[reg] / 100;
	  Memory[AddressI + 1]	= ((Registers[reg] / 10) % 10);
	  Memory[AddressI + 2]	= Registers[reg] % 10;

    PRINT("Store BCD\n");
  }

  void Chip8::OpcodeFX55()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);

    for(int i = 0; i <= reg; ++i)
      Memory[AddressI + i] = Registers[i];

    //AddressI += reg + 1;

    PRINT("Write Register to Memory FX55\n");
  }

  void Chip8::OpcodeFX65()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);

    for(int i = 0; i <= reg; ++i)
      Registers[i] = Memory[AddressI + i];

    //AddressI += reg + 1;

    PRINT("Fill Register FX65\n");
  }
}
