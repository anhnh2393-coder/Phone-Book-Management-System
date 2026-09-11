@echo off
cd /d "%~dp0"
set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if not exist server.exe (
  echo Dang bien dich server.cpp...
  set "MYSQL_CONNECTOR_INCLUDE=C:\msys64\ucrt64\include\mysql"
  set "MYSQL_CONNECTOR_LIB=C:\msys64\ucrt64\lib"
  g++ -std=c++17 server.cpp -o server.exe -lws2_32 -I"C:\msys64\ucrt64\include\mysql" -L"C:\msys64\ucrt64\lib" -lmariadb
  if errorlevel 1 (
    echo Khong tim thay g++ hoac bien dich that bai.
    pause
    exit /b 1
  )
)
server.exe
pause
