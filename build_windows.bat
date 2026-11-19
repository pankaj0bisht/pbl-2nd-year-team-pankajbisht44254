@echo off
cd /d "%~dp0"
echo Building Overparking Resolver...
g++ -std=c++11 -Wall -Wextra -pedantic CPP\backend.cpp -o parking_server.exe -lws2_32
if errorlevel 1 (
    echo.
    echo Build failed. Check that MinGW-w64 UCRT64 is installed and g++ is in PATH.
    pause
    exit /b 1
)
echo.
echo Build successful. Run run_windows.bat next.
pause
