@echo off
echo ==========================================
echo   nano1602 - PC Monitor (Windows Edition)
echo ==========================================
echo.

REM Check if Python is installed
where python >nul 2>&1
if %errorlevel% neq 0 (
    echo [!] Python 3 not found!
    echo.
    echo Please install Python from https://www.python.org/downloads/
    echo IMPORTANT: Check "Add Python to PATH" during installation.
    echo.
    pause
    exit /b 1
)

echo [*] Checking dependencies...
python -m pip install pyserial psutil --quiet

echo.
echo [*] Starting monitor...
python sender.py
pause
