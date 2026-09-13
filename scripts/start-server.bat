@echo off
cd /d "%~dp0.."
set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if not exist build mkdir build
echo Dang bien dich backend\server.cpp...
g++ -std=c++17 backend\server.cpp -o build\server.exe -lws2_32 -I"C:\msys64\ucrt64\include\mysql" -L"C:\msys64\ucrt64\lib" -lmariadb
if errorlevel 1 (
  echo Khong tim thay g++ hoac bien dich that bai.
  pause
  exit /b 1
)
build\server.exe
pause
