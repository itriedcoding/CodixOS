@echo off
REM CodixOS Build Script for Windows
REM Requires MinGW or similar GCC for Windows

echo.
echo        _____      _               ____   _____ 
echo       / ____|    | |             |  _ \ / ____^|
echo      ^| ^|     ___ ^| ^| ___  _ __   ^| ^|_) ^| (___  
echo      ^| ^|    / _ \^| ^|/ _ \^| '__^|  ^|  _ ^< \___ \ 
echo      ^| ^|___^| (_) ^| ^| (_) ^| ^|     ^| ^|_) ^|____) ^|
echo       \_____\___/^|_^|\___/^|_^|     ^|____/^|_____/ 
echo.
echo  CodixOS Build Script v1.0.0
echo.

REM Check for GCC
where gcc >nul 2>&1
if %errorLevel% neq 0 (
    echo Error: GCC is not installed or not in PATH.
    echo Please install MinGW-w64 or similar GCC distribution.
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build
if not exist "build\bin" mkdir build\bin

echo Building CodixOS Shell...
gcc -Wall -Wextra -O2 -I shell\include -I utils\include -c shell\src\codix_shell.c -o build\codix_shell.o
gcc -Wall -Wextra -O2 -I shell\include -I utils\include -c shell\src\commands.c -o build\commands.o
gcc -Wall -Wextra -O2 -I shell\include -I utils\include -c shell\src\parser.c -o build\parser.o
gcc -Wall -Wextra -O2 -I shell\include -I utils\include -c shell\src\builtins.c -o build\builtins.o
gcc -o build\bin\codix-sh.exe build\codix_shell.o build\commands.o build\parser.o build\builtins.o
echo Shell built: build\bin\codix-sh.exe

echo Building System Utilities...
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-ls.exe utils\src\ls.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-cat.exe utils\src\cat.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-cp.exe utils\src\cp.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-mv.exe utils\src\mv.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-rm.exe utils\src\rm.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-mkdir.exe utils\src\mkdir.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-echo.exe utils\src\echo.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-grep.exe utils\src\grep.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-chmod.exe utils\src\chmod.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-ps.exe utils\src\ps.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-kill.exe utils\src\kill.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-df.exe utils\src\df.c
gcc -Wall -Wextra -O2 -I utils\include -o build\bin\codix-free.exe utils\src\free.c
echo Utilities built in build\bin\

echo Building Terminal Emulator...
gcc -Wall -Wextra -O2 -I terminal\include -o build\bin\codix-term.exe terminal\src\terminal.c
echo Terminal built: build\bin\codix-term.exe

echo Building Package Manager...
gcc -Wall -Wextra -O2 -I pkgmanager\include -o build\bin\codix-pkg.exe pkgmanager\src\codix_pkg.c
echo Package manager built: build\bin\codix-pkg.exe

echo.
echo Building Desktop Environment Components...
echo.

REM Build desktop components (stubs for Windows)
echo Building Window Manager...
gcc -Wall -Wextra -O2 -I desktop\wm\include -c desktop\wm\src\windowmanager.c -o build\windowmanager.o 2>nul
if %errorLevel% equ 0 (
    echo Window manager compiled
) else (
    echo Window manager: requires SDL2 for full GUI support
)

echo Building Desktop Shell...
gcc -Wall -Wextra -O2 -I desktop\shell\include -c desktop\shell\src\desktopshell.c -o build\desktopshell.o 2>nul
if %errorLevel% equ 0 (
    echo Desktop shell compiled
) else (
    echo Desktop shell: requires SDL2 for full GUI support
)

echo Building GUI Terminal...
gcc -Wall -Wextra -O2 -I desktop\apps\terminal\include -c desktop\apps\terminal\src\guiterminal.c -o build\guiterminal.o 2>nul
if %errorLevel% equ 0 (
    echo GUI terminal compiled
) else (
    echo GUI terminal: requires SDL2 for full GUI support
)

echo Building File Manager...
gcc -Wall -Wextra -O2 -I desktop\apps\filemanager\include -c desktop\apps\filemanager\src\filemanager.c -o build\filemanager.o 2>nul
if %errorLevel% equ 0 (
    echo File manager compiled
) else (
    echo File manager: requires SDL2 for full GUI support
)

echo Building Text Editor...
gcc -Wall -Wextra -O2 -I desktop\apps\texteditor\include -c desktop\apps\texteditor\src\texteditor.c -o build\texteditor.o 2>nul
if %errorLevel% equ 0 (
    echo Text editor compiled
) else (
    echo Text editor: requires SDL2 for full GUI support
)

echo Building Settings Panel...
gcc -Wall -Wextra -O2 -I desktop\apps\settings\include -c desktop\apps\settings\src\settings.c -o build\settings.o 2>nul
if %errorLevel% equ 0 (
    echo Settings compiled
) else (
    echo Settings: requires SDL2 for full GUI support
)

echo Building System Monitor...
gcc -Wall -Wextra -O2 -I desktop\apps\monitor\include -c desktop\apps\monitor\src\systemmonitor.c -o build\systemmonitor.o 2>nul
if %errorLevel% equ 0 (
    echo System monitor compiled
) else (
    echo System monitor: requires SDL2 for full GUI support
)

echo Building Calculator...
gcc -Wall -Wextra -O2 -I desktop\apps\calculator\include -c desktop\apps\calculator\src\calculator.c -o build\calculator.o 2>nul
if %errorLevel% equ 0 (
    echo Calculator compiled
) else (
    echo Calculator: requires SDL2 for full GUI support
)

echo.
echo Copying configuration files...
if not exist "build\share\codixos" mkdir build\share\codixos
xcopy /E /I /Y desktop\config\* build\share\codixos\ 2>nul
xcopy /E /I /Y desktop\theme\* build\share\codixos\ 2>nul

echo.
echo ========================================
echo   Build Complete!
echo ========================================
echo.
echo Build artifacts are in: build\bin\
echo.
echo Components built:
echo   - CodixOS Shell (codix-sh.exe)
echo   - System Utilities (codix-*.exe)
echo   - Terminal Emulator (codix-term.exe)
echo   - Package Manager (codix-pkg.exe)
echo   - Desktop Environment (requires SDL2)
echo.
echo To run CodixOS Shell:
echo   build\bin\codix-sh.exe
echo.
echo To run Desktop Environment (requires SDL2 and X11/WSL):
echo   ./scripts/start-desktop.sh
echo.
pause
