# Проект для Visual Studio / CLion: «Открыть папку» — среда сама подхватит этот файл
cmake_minimum_required(VERSION 3.10)
project(Workers CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(workers
    src/main.cpp
    src/core/Person.cpp
    src/entities/Worker.cpp
    src/container/WorkerList.cpp
    src/text/TextDocument.cpp
    src/exceptions/AppException.cpp
    src/input/Input.cpp
    src/utils/TextUtils.cpp
)

if(MSVC)
    # Исходники в UTF-8: без этого флага MSVC испортит русские строки
    target_compile_options(workers PRIVATE /utf-8 /W4)
else()
    target_compile_options(workers PRIVATE -Wall -Wextra)
endif()
