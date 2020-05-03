@echo off

IF NOT EXIST ..\build mkdir ..\build
cd ..\build 

cl -LD ..\src\game.cpp /link /EXPORT:UpdateAndRender
cl ..\src\windows_main.cpp ..\external_lib\SDL2main.lib ..\external_lib\SDL2.lib

cd ..\src