echo OFF

set SDCC_DIR=%CD%\toolStack\sdcc-20260330
set NINJA_DIR=%CD%\toolStack\ninja-1.13.2
set MISC_DIR=%CD%\toolStack\misc
set PATH=%NINJA_DIR%;%SDCC_DIR%\bin;%MISC_DIR%;%PATH%

del %CD%\build\*.* /Q /F

ninja

rem If error, let's exit
if errorlevel 1 (
   echo Build error !!!
   exit /b %errorlevel%
)
