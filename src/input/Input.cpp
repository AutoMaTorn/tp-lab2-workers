#include "Input.h"
#include "../exceptions/AppException.h"

#include <iostream>
#include <sstream>

namespace Input {

std::string readString(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw AppException("Входной поток закрыт");
    }
    if (!line.empty() && line[line.size() - 1] == '\r') {
        line.erase(line.size() - 1);
    }
    return line;
}

int readInt(const std::string& prompt, int min, int max) {
    while (true) {
        std::string line = readString(prompt);
        // Разбор строки строковым потоком: число, после которого только пробелы
        std::istringstream in(line);
        long value = 0;
        char rest = 0;
        if (!(in >> value) || (in >> rest)) {
            std::cout << "Ошибка: введите целое число" << std::endl;
        } else if (value < min || value > max) {
            std::cout << "Ошибка: число должно быть от " << min << " до " << max << std::endl;
        } else {
            return static_cast<int>(value);
        }
    }
}

}
