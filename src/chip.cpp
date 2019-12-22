#include "chip.h"

namespace chip
{
  Chip8::Chip8()
  {
    Memory = new uint8_t[0x1000];

    PC = 0x200;
    SP = 0;

    IsWorking = true;
  }

  Chip8::~Chip8()
  {
    delete[] Memory;
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

    fread(&Memory[0x200], 0x1000, 1, file);
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
          case 0x0000: Opcode8XY0(); break;
          case 0x0001: Opcode8XY1(); break;
          case 0x0002: Opcode8XY2(); break;
          case 0x0003: Opcode8XY3(); break;
          case 0x0004: Opcode8XY4(); break;
          case 0x0005: Opcode8XY5(); break;
          case 0x0006: Opcode8XY6(); break;
          case 0x0007: Opcode8XY7(); break;
          case 0x000E: Opcode8XYE(); break;
        }
      }break;
    }
  }


  /*  Instructions  */

  void Chip8::ClearScreen()
  {

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

    if(Registers[reg] == Opcode & 0x00FF)
      PC += 2;

    PRINT("Condition 3XNN\n");
  }

  void Chip8::Opcode4XNN()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);

    if(Registers[reg] != Opcode & 0x00FF)
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

    PRINT("Fill Register\n");
  }

  void Chip8::Opcode7XNN()
  {
    uint8_t reg = ((Opcode & 0x0F00) >> 8);
    Registers[reg] += Opcode & 0x00FF;

    PRINT("Update Register value\n");
  }

  void Chip8::Opcode8XY0()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    Registers[regX] = Registers[regY];

    PRINT("Set Register value\n");
  }

  void Chip8::Opcode8XY1()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    Registers[regX] |= Registers[regY];

    PRINT("Update Register value\n");
  }

  void Chip8::Opcode8XY2()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    Registers[regX] &= Registers[regY];

    PRINT("Update Register value\n");
  }

  void Chip8::Opcode8XY3()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    Registers[regX] ^= Registers[regY];

    PRINT("Update Register value\n");
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

    PRINT("Update Register value\n");
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

    PRINT("Update Register value\n");
  }

  void Chip8::Opcode8XY6()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);

    Registers[0xF] = Registers[regX] & 0x1;
    Registers[regX] >>= 1;

    PRINT("Update Register value\n");
  }

  void Chip8::Opcode8XY7()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);
    uint8_t regY = ((Opcode & 0x00F0) >> 4);

    if(Registers[regY] < Registers[regX])
      Registers[0xF] = 0;
    else
      Registers[0xF] = 1;

    Registers[regX] = Registers[regY] - Registers[regX];

    PRINT("Update Register value\n");
  }

  void Chip8::Opcode8XYE()
  {
    uint8_t regX = ((Opcode & 0x0F00) >> 8);

    Registers[0xF] = Registers[regX] >> 7;
    Registers[regX] <<= 1;

    PRINT("Update Register value\n");
  }sw
}
