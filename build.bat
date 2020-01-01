@echo off
cd bin
g++ ..\src\main.cpp ..\src\chip.cpp -I ..\extern\SDL2-2.0.10\i686-w64-mingw32\include -L ..\extern\SDL2-2.0.10\i686-w64-mingw32\lib -lmingw32 -lSDL2main -lSDL2  -o "Chip8-Emulator"
