#include <memory>
#include <cstdint>
#include <cassert>
#include <fstream>

namespace chip
{
  #define ASSERT(x, y) assert(x && y)
  #define PRINT(x) fprintf(stderr, x);

  class Chip8
  {
  public:
    uint8_t*    Memory;
    uint8_t     Registers[0x10];
    uint16_t    Stack[0x10];

    uint16_t AddressI;
    uint16_t Opcode;
    uint16_t SP;
    uint16_t PC;

    bool IsWorking;

    explicit Chip8();
    ~Chip8();

    //Load new opcode from memory
    void FetchInstruction();

    //Load game instructions
    //Parametr is path to the game ROM
    void LoadRom(const char* filepath);

    //Decide which instruction execute
    void DecodeInstruction();


    /*    Instructions  */

    //Clear the screen buffer. Opcode 00E0
    void ClearScreen();

    //Return from subroutine
    void Opcode00EE();

    //Jump to address NNN
    void Opcode1NNN();

    //Call subroutine at address NNN
    void Opcode2NNN();

    //Skip next instruction if Register X equal to NN
    void Opcode3XNN();

    //Skip next instruction if Register X  doesn't equal to NN
    void Opcode4XNN();

    //Skip the next instruction if Register X equal to Register Y
    void Opcode5XY0();

    //Set Register X to value NN
    void Opcode6XNN();

    //Add value NN to Register X
    void Opcode7XNN();

    //Set Register X to the value of Register Y
    void Opcode8XY0();

    //Set Register X to the value Register X  | Register Y
    void Opcode8XY1();

    //Set Register X to the value Register X | Register Y
    void Opcode8XY2();

    //Set Register X to the value Register X ^ Register Y
    void Opcode8XY3();

    //Add Register X to Register Y, when there's a carry Register F set to 1
    void Opcode8XY4();

    //Subtract Register Y  from Register X, when there's a borrow Register F set to 0
    void Opcode8XY5();

    //Store the LSB of Register X in Register F and shift Register X to the right by 1
    void Opcode8XY6();

    //Set Register X to Register Y minus Register X, set Register F to 0 when thre's borrow
    void Opcode8XY7();

    //Store MSB of Register X in Register F and shift to the left by 1 Register X
    void Opcode8XYE();

    Chip8(const Chip8&)               = delete;
    Chip8& operator = (const Chip8&)  = delete;
  };
}
