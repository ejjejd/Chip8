#define MEMORY_SIZE         4096
#define REGISTERS_COUNT     16
#define STACK_SIZE          16
#define KEYS_COUNT          16
#define ORIGINAL_SCREEN_X   64
#define ORIGINAL_SCREEN_Y   32
#define START_ADDRESS       512
#define FONTSET_SIZE        80

#define INSTRUCTION_PER_SECOND  1000
#define SLEEP_TIME              1'000'000 / INSTRUCTION_PER_SECOND

#ifdef DEBUG
  #define ASSERT(x, y) assert(x && y)
  #define PRINT(x) fprintf(stderr, x)
#else
  #define ASSERT(x, y) if(!(x))exit(-1)
  #define PRINT(x)
#endif
