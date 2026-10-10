@echo off
set PATH=C:\msys64\ucrt64\bin;%PATH%
if not exist train_reservation.exe (
    echo train_reservation.exe not found! Building first...
    call build.bat
)
echo Launching Train Reservation Management System...
echo.
train_reservation.exe
pause
