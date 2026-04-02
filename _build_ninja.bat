echo OFF

set SDCC_DIR=%CD%\toolStack\sdcc-20260330
set NINJA_DIR=%CD%\toolStack\ninja-1.13.2
set EMU_DIR=%CD%\toolStack\speccy-5.9
set MISC_DIR=%CD%\toolStack\misc
set PATH=%NINJA_DIR%;%EMU_DIR%;%SDCC_DIR%\bin;%MISC_DIR%;%PATH%

del %CD%\build\*.* /Q /F

ninja

rem If error, let's exit
if errorlevel 1 (
   echo Build error !!!
   exit /b %errorlevel%
)

rem ------ Move all *.lis files (dissambled) to the Build folder
rem for /r ".\src" %%A in (*.lis) do (
rem   move "%%A" ".\build"
rem )

rem ------- If Tap => Size + MemReport + Emulator
rem if exist ".\build\%APPNAME%.tap" (
rem   forfiles /p ".\build" /m "%APPNAME%.tap" /c "cmd /c echo ####### size: @fsize #######"
rem   dir .\build\*.tap
rem   dir .\build\*.tap >> compilations.log
rem   Speccy.exe "%CD%\build\%APPNAME%.tap"
rem ) else (
rem   echo Error with %APPNAME%.tap !!!
rem )

