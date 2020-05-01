@echo off

IF NOT EXIST ..\build mkdir ..\build
cd ..\build 


echo "---------LIB---------"
cl /LD /FC ..\src\game.cpp /link /EXPORT:moveRect
echo "---------EXE---------"
cl /Zi /FC ..\src\windows_main.cpp ..\lib\SDL2main.lib ..\lib\SDL2.lib ..\lib\SDL2_mixer.lib 
echo "---------END---------"
cd ..\src