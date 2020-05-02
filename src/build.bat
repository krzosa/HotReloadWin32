@echo off

set CompilerFlags=-nologo -MT -Oi -FC -W4 -wd4100 -Zi
set LinkerFlags=..\lib\SDL2main.lib ..\lib\SDL2.lib ..\lib\SDL2_mixer.lib

IF NOT EXIST ..\build mkdir ..\build
cd ..\build 


echo "---------LIB---------"
del *.pdb > NUL 2> NUL
cl %CompilerFlags% -LD -Fdgame%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb ..\src\game.cpp /link -incremental:no -opt:ref /EXPORT:UpdateAndRender
echo "---------EXE---------"
cl %CompilerFlags% ..\src\windows_main.cpp %LinkerFlags%

cd ..\src