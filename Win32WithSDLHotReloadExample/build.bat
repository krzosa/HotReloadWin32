@echo off

mkdir build
cd build 

cl -LD ..\game.cpp /link /EXPORT:UpdateAndRender
cl ..\windows_main.cpp ..\external_lib\SDL2main.lib ..\external_lib\SDL2.lib

cd ..