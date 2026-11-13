@echo off

:: NOTE: For the ctime timings make sure to change the name
:: of the file here and at the end of the script as well
ctime -begin jam.ctm

set CompilerFlags= /Od /Zi /MDd /FC /nologo /D DEBUG
REM set LinkerFlags=-subsystem:Console

IF NOT EXIST build mkdir build

pushd build

:: NOTE: This only needs to run for the first compile and then 
:: it can be commented out for future builds to dramatically increase compile times.
:: cl %CompilerFlags% /c /D PLATFORM_DESKTOP /D DEBUG /I..\external\Raylib\external\glfw\include ^
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

ctime -end jam.ctm
