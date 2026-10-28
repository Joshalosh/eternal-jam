@echo off

:: NOTE: For the ctime timings make sure to change the name
:: of the file here and at the end of the script as well
::ctime -begin jam.ctm

set CompilerFlags= /Zi /MDd /FC /nologo
REM set LinkerFlags=-subsystem:Console

IF NOT EXIST ..\build mkdir ..\build

pushd ..\build

cl %CompilerFlags% /c /D PLATFORM_DESKTOP /I..\external\Raylib\external\glfw\include ^
    ..\external\raylib\rcore.c ^
    ..\external\raylib\rmodels.c ^
    ..\external\raylib\raudio.c ^
    ..\external\raylib\rglfw.c ^
    ..\external\raylib\rshapes.c ^
    ..\external\raylib\rtext.c ^
    ..\external\raylib\rtextures.c

:: NOTE: Make sure to name both the .cpp and .exe files appropriately
cl %CompilerFlags% ^
    ..\code\jam.cpp ^
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

::ctime -end jam.ctm
