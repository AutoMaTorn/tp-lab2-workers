#!/bin/sh
# Сборка программы компилятором g++. Результат: build/workers(.exe)
set -e
cd "$(dirname "$0")"
mkdir -p build
g++ -std=c++17 -Wall -Wextra $(find src -name '*.cpp') -o build/workers
echo "Готово: build/workers"
