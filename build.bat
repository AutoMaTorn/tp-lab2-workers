@echo off
rem Build with g++ (MinGW / MSYS2). Output: build\workers.exe
if not exist build mkdir build
g++ -std=c++17 -Wall -Wextra ^
    src\main.cpp ^
    src\core\Person.cpp ^
    src\entities\Worker.cpp ^
    src\container\WorkerList.cpp ^
    src\text\TextDocument.cpp ^
    src\exceptions\AppException.cpp ^
    src\input\Input.cpp ^
    src\utils\TextUtils.cpp ^
    -o build\workers.exe
if errorlevel 1 (
    echo Build failed
    exit /b 1
)
echo Done: build\workers.exe
