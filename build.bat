@echo off
set PATH=C:\msys64\ucrt64\bin;%PATH%
echo Compiling Train Reservation Management System...
gcc -Wall -Wextra -std=c99 src/main.c -o train_reservation.exe
if %ERRORLEVEL% equ 0 (
    echo.
    echo ========================================================
    echo Build Successful: train_reservation.exe generated!
    echo Run using: .\train_reservation.exe or run.bat
    echo ========================================================
) else (
    echo.
    echo Compilation failed. Please check error messages above.
)
pause
