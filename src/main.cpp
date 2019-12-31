#include "chip.h"
#include "chip_utility.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

chip::Chip8 chipPtr;

uint8_t Keymap[16] =
{
  SDLK_x, SDLK_1, SDLK_2, SDLK_3,
  SDLK_q, SDLK_w, SDLK_e, SDLK_a,
  SDLK_s, SDLK_d, SDLK_z, SDLK_c,
  SDLK_4, SDLK_r, SDLK_f, SDLK_v,
};

int main(int argc, char* argv[])
{
  SDL_Window* window = nullptr;

  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    fprintf(stderr, "SDL could not initialize! Error: %s\n", SDL_GetError());
    return -1;
  }

  window = SDL_CreateWindow("Chip8-Emualtor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_SHOWN);

  if (window == nullptr)
	{
		fprintf(stderr, "Window could not be created! Error: %s\n", SDL_GetError());
		return -1;
	}

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

  SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

  uint32_t pixels[2048];

  chipPtr.LoadRom(argv[1]);

  while(chipPtr.IsWorking)
  {
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        if (e.type == SDL_KEYDOWN)
        {
          for (int i = 0; i < 16; ++i)
            if (e.key.keysym.sym == Keymap[i])
              chipPtr.Keys[i] = 1;
        }

        if (e.type == SDL_KEYUP)
        {
          for (int i = 0; i < 16; ++i)
            if (e.key.keysym.sym == Keymap[i])
              chipPtr.Keys[i] = 0;
        }
    }

    chipPtr.FetchInstruction();
    chipPtr.DecodeInstruction();

    for(int i = 0; i < 64; ++i)
    {
    		for (int j = 0; j < 32; ++j)
    		{
    		    uint8_t pixel = chipPtr.Screen[i][j];
    				pixels[i + j * 64] = (0x00FFFFFF * pixel) | 0xFF000000;
    		}
    }

    SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(Uint32));
		SDL_RenderClear(renderer);

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

    //Need thread extension
    //std::this_thread::sleep_for(std::chrono::microseconds(1200));
  }

  return 0;
}
