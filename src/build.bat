@echo off

cd ..\build 
cl /LD ..\src\main.cpp /link /EXPORT:moveRect
cl /Zi ..\src\windows_main.cpp ..\lib\SDL2main.lib ..\lib\SDL2.lib ..\lib\SDL2_mixer.lib 
cd ..\src