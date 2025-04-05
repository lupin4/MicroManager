@echo off
setlocal enabledelayedexpansion

REM === CONFIGURATION ===
set RIDER_PATH=C:\Users\DiscoMachine\AppData\Local\Programs\Rider\bin\rider64.exe
set VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Preview\Common7\IDE\devenv.exe

set UE_5_3_EDITOR=G:\EpicGames\UE\UE_5.3\Engine\Binaries\Win64\UnrealEditor.exe
set UE_5_5_EDITOR=G:\EpicGames\UE\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe

set VS_COMMUNITY_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community
set VS_PREVIEW_PATH=C:\Program Files\Microsoft Visual Studio\2022\Preview

set MSVC_UE53=14.38.33130
set MSVC_UE55=14.44.34918

set LOGFILE="%CD%\msvc_log.txt"
set XML_PATH=%AppData%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml
REM ======================

REM === Locate .uproject ===
for %%f in (*.uproject) do set UPROJECT=%%f
if not defined UPROJECT (
    echo ERROR: No .uproject file found.
    pause
    exit /b 1
)

REM === Ask for UE version ===
echo Select the Unreal Engine version for this project:
echo [1] UE 5.3
echo [2] UE 5.5
set /p ue_choice="Enter choice (1 or 2): "

if "%ue_choice%"=="1" (
    set UE_VER=5.3
    set VCTOOLS_VERSION=%MSVC_UE53%
    set SELECTED_VS=%VS_COMMUNITY_PATH%
    set UE_EDITOR=%UE_5_3_EDITOR%
) else (
    set UE_VER=5.5
    set VCTOOLS_VERSION=%MSVC_UE55%
    set SELECTED_VS=%VS_PREVIEW_PATH%
    set UE_EDITOR=%UE_5_5_EDITOR%
)

REM === Set PATH for MSVC ===
set "VCToolsInstallDir=%SELECTED_VS%\VC\Tools\MSVC\%VCTOOLS_VERSION%"
set "PATH=%VCToolsInstallDir%\bin\Hostx64\x64;%PATH%"

echo [%DATE% %TIME%] Using MSVC: %VCTOOLS_VERSION% >> %LOGFILE%
echo MSVC version set to %VCTOOLS_VERSION%
echo Toolchain path: %VCToolsInstallDir%
echo Log saved to: %LOGFILE%
echo.

REM === Write BuildConfiguration.xml dynamically ===
echo Generating BuildConfiguration.xml...

(
  echo ^<?xml version="1.0" encoding="utf-8"?^>
  echo ^<Configuration xmlns="https://www.unrealengine.com/BuildConfiguration"^>
  echo   ^<WindowsPlatform^>
  echo     ^<Compiler^>VisualStudio2022^</Compiler^>
  echo     ^<CompilerVersion^>%VCTOOLS_VERSION%^</CompilerVersion^>
  echo   ^</WindowsPlatform^>
  echo ^</Configuration^>
) > "%XML_PATH%"

echo BuildConfiguration.xml updated at: %XML_PATH%
echo.

REM === Locate .sln file ===
for %%f in (*.sln) do set SLN=%%f
if not defined SLN (
    echo ERROR: No .sln file found.
    pause
    exit /b 1
)

REM === Confirm paths before launching ===
echo Project folder: %CD%
echo UPROJECT file:  %UPROJECT%
echo SLN file:       %SLN%
echo Rider path:     %RIDER_PATH%
echo Visual Studio:  %VS_PATH%
echo UE Editor path: %UE_EDITOR%
echo.

REM === Launch options ===
echo Choose launch mode:
echo [1] Rider only
echo [2] Visual Studio only
echo [3] Rider + Unreal Editor
echo [4] Visual Studio + Unreal Editor
set /p choice="Your choice (1-4): "

if "%choice%"=="1" (
    echo Launching Rider...
    call "%RIDER_PATH%" "%CD%\%SLN%"
)
if "%choice%"=="2" (
    echo Launching Visual Studio...
    call "%VS_PATH%" "%CD%\%SLN%"
)
if "%choice%"=="3" (
    echo Launching Rider and Unreal Editor...
    start "" "%RIDER_PATH%" "%CD%\%SLN%"
    call "%UE_EDITOR%" "%CD%\%UPROJECT%" -log
)
if "%choice%"=="4" (
    echo Launching Visual Studio and Unreal Editor...
    call "%VS_PATH%" "%CD%\%SLN%"
    call "%UE_EDITOR%" "%CD%\%UPROJECT%" -log
)

echo Done.
pause
exit /b 0
