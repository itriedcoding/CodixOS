@echo off
REM CodixOS Windows Installer
REM Installs CodixOS as a WSL distribution or standalone application

echo.
echo        _____      _               ____   _____ 
echo       / ____|    ^| ^|             ^|  _ \ / ____^|
echo      ^| ^|     ___ ^| ^| ___  _ __   ^| ^|_) ^| (___  
echo      ^| ^|    / _ \^| ^|/ _ \^| '__^|  ^|  _ ^< \___ \ 
echo      ^| ^|___^| (_) ^| ^| (_) ^| ^|     ^| ^|_) ^|____) ^|
echo       \_____\___/^|_^|\___/^|_^|     ^|____/^|_____/ 
echo.
echo  CodixOS Installer v1.0.0
echo  ========================
echo.

REM Check for administrator privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Error: This installer requires administrator privileges.
    echo Please run as administrator.
    pause
    exit /b 1
)

REM Display menu
echo Select installation method:
echo.
echo   1. Install as WSL Distribution
echo   2. Install as Standalone Application
echo   3. Create Desktop Shortcut
echo   4. Uninstall
echo   5. Exit
echo.
set /p choice="Enter your choice (1-5): "

if "%choice%"=="1" goto install_wsl
if "%choice%"=="2" goto install_standalone
if "%choice%"=="3" goto create_shortcut
if "%choice%"=="4" goto uninstall
if "%choice%"=="5" goto exit
echo Invalid choice. Please try again.
goto exit

:install_wsl
echo.
echo Installing CodixOS as WSL Distribution...
echo.

REM Check if WSL is enabled
wsl --list >nul 2>&1
if %errorLevel% neq 0 (
    echo Enabling Windows Subsystem for Linux...
    dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
    dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
    echo.
    echo Please restart your computer and run this installer again.
    pause
    goto exit
)

REM Import CodixOS as WSL distribution
echo Importing CodixOS...
wsl --import CodixOS "%USERPROFILE%\CodixOS" "%~dp0images\codixos-wsl.tar"

if %errorLevel% equ 0 (
    echo.
    echo CodixOS has been installed as a WSL distribution.
    echo You can now run: wsl -d CodixOS
    echo.
) else (
    echo Error: Failed to install CodixOS.
)
pause
goto exit

:install_standalone
echo.
echo Installing CodixOS as Standalone Application...
echo.

REM Create installation directory
set INSTALL_DIR=%ProgramFiles%\CodixOS
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

REM Copy files
echo Copying files...
copy "%~dp0bin\codixos.exe" "%INSTALL_DIR%\"
copy "%~dp0bin\*.dll" "%INSTALL_DIR%\" 2>nul
copy "%~dp0etc\*" "%INSTALL_DIR%\etc\" /s /e /y 2>nul
copy "%~dp0usr\*" "%INSTALL_DIR%\usr\" /s /e /y 2>nul

REM Add to PATH
echo Adding to PATH...
setx PATH "%PATH%;%INSTALL_DIR%" /M

echo.
echo CodixOS has been installed to: %INSTALL_DIR%
echo You can now run: codixos
echo.
pause
goto exit

:create_shortcut
echo.
echo Creating Desktop Shortcut...
echo.

REM Create shortcut
set SCRIPT_DIR=%~dp0
set SHORTCUT_PATH=%USERPROFILE%\Desktop\CodixOS.lnk

powershell -Command "$ws = New-Object -ComObject WScript.Shell; $sc = $ws.CreateShortcut('%SHORTCUT_PATH%'); $sc.TargetPath = '%SCRIPT_DIR%bin\codixos.exe'; $sc.WorkingDirectory = '%SCRIPT_DIR%bin'; $sc.Description = 'CodixOS Terminal'; $sc.Save()"

echo Desktop shortcut created.
pause
goto exit

:uninstall
echo.
echo Uninstalling CodixOS...
echo.

REM Remove WSL distribution
wsl --unregister CodixOS 2>nul

REM Remove installation directory
if exist "%ProgramFiles%\CodixOS" (
    rmdir /s /q "%ProgramFiles%\CodixOS"
)

REM Remove desktop shortcut
if exist "%USERPROFILE%\Desktop\CodixOS.lnk" (
    del "%USERPROFILE%\Desktop\CodixOS.lnk"
)

echo CodixOS has been uninstalled.
pause
goto exit

:exit
echo.
echo Thank you for using CodixOS!
echo.
