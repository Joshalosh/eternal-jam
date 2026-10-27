@echo off

:: NOTE: For the ctime timings make sure to change the name
:: of the file here and at the end of the script as well
ctime -begin jam.ctm

set CompilerFlags= /Zi /MDd /FC /nologo
REM set LinkerFlags=-subsystem:Console

IF NOT EXIST ..\build mkdir ..\build

pushd ..\build

cl %CompilerFlags% /c /D PLATFORM_DESKTOP /I..\external\Raylib\external\glfw\include ^
    ..\external\Raylib\rcore.c ^
    ..\external\Raylib\rmodels.c ^
    ..\external\Raylib\raudio.c ^
    ..\external\Raylib\rglfw.c ^
    ..\external\Raylib\rshapes.c ^
    ..\external\Raylib\rtext.c ^
    ..\external\Raylib\rtextures.c

:: NOTE: Make sure to name both the .cpp and .exe files appropriately
cl %CompilerFlags% ^
    ..\jam.cpp ^
    rcore.obj ^
    rmodels.obj ^
    raudio.obj ^
    rglfw.obj ^
    rshapes.obj ^
    rtext.obj ^
    rtextures.obj ^
    /I ..\include/ /link /FORCE:MULTIPLE -incremental:no %LinkerFlags% -out:jam.exe ^
    Gdi32.lib ^
    winmm.lib ^
    user32.lib ^
    shell32.lib

popd

ctime -end jam.ctm
