@echo off
chcp 65001 >nul
title nano1602 - PC Monitor

:: ========================================== 
::   nano1602 - PC Monitor (Windows Edition) 
:: ========================================== 

echo.
echo ==========================================
echo   nano1602 - PC Monitor (Windows Edition)
echo ==========================================
echo.

REM Try 'py' first (Python Launcher), then 'python'
where py >nul 2>&1
if %errorlevel% equ 0 (
    set PYTHON_CMD=py
    goto :run
)

where python >nul 2>&1
if %errorlevel% equ 0 (
    set PYTHON_CMD=python
    goto :run
)

echo [!] Python 3 not found!
echo.
echo Please install Python from https://www.python.org/downloads/
echo IMPORTANT: Check "Add Python to PATH" during installation.
echo.
pause
exit /b 1

:run
echo [*] Installed Python version:
%PYTHON_CMD% --version
echo.

echo [*] Installing dependencies (pyserial, psutil)...
%PYTHON_CMD% -m pip install pyserial psutil --quiet

echo.
echo [*] Starting monitor... (Ctrl+C to stop)
echo ------------------------------------------
%PYTHON_CMD% sender.py

echo.
pause
