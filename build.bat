@echo off
cd bin
g++ ..\src\main.cpp ..\src\chip.cpp ..\src\audio\openal\openal.cpp ..\src\audio\audio.cpp -I ..\extern\SDL2-2.0.10\i686-w64-mingw32\include -I ..\extern\OpenAL\include -L ..\extern\OpenAL\libs\Win32 -L ..\extern\SDL2-2.0.10\i686-w64-mingw32\lib -lmingw32 -lSDL2main -lSDL2 -lOpenAL32 -o "Chip8-Emulator"
