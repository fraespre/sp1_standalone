set SDCC_DIR=%CD%\toolStack\sdcc-20260228
set NINJA_DIR=%CD%\toolStack\ninja-1.13.2
set EMU_DIR=%CD%\toolStack\speccy-5.9
set MISC_DIR=%CD%\toolStack\misc

set PATH=%NINJA_DIR%;%EMU_DIR%;%SDCC_DIR%\bin;%MISC%;%PATH%

start cmd /k "sdcc -version & z88dk-zsdcc -v"

