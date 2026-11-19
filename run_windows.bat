@echo off
cd /d "%~dp0"
if not exist parking_server.exe (
    echo parking_server.exe not found. Run build_windows.bat first.
    pause
    exit /b 1
)
parking_server.exe
pause
