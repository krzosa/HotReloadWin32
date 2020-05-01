@echo off

IF NOT EXIST ..\build mkdir ..\build
cd ..\build 


echo "---------LIB---------"
cl -nologo -MT -Oi -LD -FC -W4 -wd4100 ..\src\game.cpp /link /EXPORT:UpdateAndRender
echo "---------EXE---------"
cl -nologo -MT -Oi -Zi -FC -W4 -wd4100 ..\src\windows_main.cpp ..\lib\SDL2main.lib ..\lib\SDL2.lib ..\lib\SDL2_mixer.lib 

cd ..\src