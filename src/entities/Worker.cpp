#include "Worker.h"
#include "../exceptions/AppException.h"
#include "../utils/TextUtils.h"

#include <ctime>
#include <sstream>

Worker::Worker() : Person(), position("Не указана"), hireYear(currentYear()) {
    std::cout << "[Worker] Вызван конструктор без параметров" << std::endl;
}

Worker::Worker(const std::string& fullName, const std::string& position, int hireYear)
    : Person(fullName), position("Не указана"), hireYear(currentYear()) {
    setPosition(position);
    setHireYear(hireYear);
    std::cout << "[Worker] Вызван конструктор с параметрами" << std::endl;
}

Worker::Worker(const Worker& other)
    : Person(other), position(other.position), hireYear(other.hireYear) {
    std::cout << "[Worker] Вызван конструктор копирования" << std::endl;
}

Worker::~Worker() {
    std::cout << "[Worker] Вызван деструктор (" << fullName << ")" << std::endl;
}

Worker& Worker::operator=(const Worker& other) {
    if (this != &other) {
        Person::operator=(other);
        position = other.position;
        hireYear = other.hireYear;
    }
    return *this;
}

bool Worker::operator==(const Worker& other) const {
    return fullName == other.fullName && position == other.position &&
           hireYear == other.hireYear;
}

bool Worker::operator<(const Worker& other) const {
    return TextUtils::compareAlpha(fullName, other.fullName) < 0;
}

std::string Worker::getPosition() const { return position; }
int Worker::getHireYear() const { return hireYear; }

void Worker::setPosition(const std::string& position) {
    std::string value = TextUtils::trim(position);
    checkPosition(value);
    this->position = value;
}

void Worker::setHireYear(int hireYear) {
    checkHireYear(hireYear);
    this->hireYear = hireYear;
}

void Worker::setAll(const std::string& fullName, const std::string& position, int hireYear) {
    std::string name = TextUtils::trim(fullName);
    std::string pos = TextUtils::trim(position);
    // Сначала проверяем всё: при ошибке объект останется без изменений
    checkFullName(name);
    checkPosition(pos);
    checkHireYear(hireYear);
    this->fullName = name;
    this->position = pos;
    this->hireYear = hireYear;
}

void Worker::checkPosition(const std::string& position) {
    if (TextUtils::isBlank(position)) {
        throw AppException("Название должности не может быть пустым");
    }
}

void Worker::checkHireYear(int hireYear) {
    int now = currentYear();
    if (hireYear < MIN_HIRE_YEAR) {
        throw AppException("Год поступления на работу не может быть раньше " +
                           std::to_string(MIN_HIRE_YEAR) + " (введено " +
                           std::to_string(hireYear) + ")");
    }
    if (hireYear > now) {
        throw AppException("Год поступления на работу не может быть больше текущего (" +
                           std::to_string(now) + "), введено " + std::to_string(hireYear));
    }
}

int Worker::getExperience() const {
    return currentYear() - hireYear;
}

int Worker::currentYear() {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    return local->tm_year + 1900;
}

std::string Worker::getRole() const { return "Работник"; }

void Worker::print(std::ostream& out) const {
    int experience = getExperience();
    out << getRole() << ": " << fullName << "\n"
        << "    Должность: " << position << "\n"
        << "    Год поступления: " << hireYear
        << " (стаж " << experience << " " << TextUtils::yearsWord(experience) << ")";
}

void Worker::save(std::ostream& out) const {
    out << fullName << "\n" << position << "\n" << hireYear << "\n";
}

std::ostream& operator<<(std::ostream& out, const Worker& worker) {
    worker.print(out);
    return out;
}

// Чтение одной строки; при вводе с клавиатуры сначала печатается подсказка
static std::string readLineFrom(std::istream& in, bool interactive, const char* prompt) {
    if (interactive) {
        std::cout << prompt;
    }
    std::string line;
    if (!std::getline(in, line)) {
        throw AppException(interactive ? "Входной поток закрыт"
                                       : "Ошибка чтения файла: данные работника обрываются");
    }
    if (!line.empty() && line[line.size() - 1] == '\r') {
        line.erase(line.size() - 1);
    }
    return line;
}

std::istream& operator>>(std::istream& in, Worker& worker) {
    bool interactive = (&in == &std::cin);
    std::string fullName = readLineFrom(in, interactive, "ФИО: ");
    std::string position = readLineFrom(in, interactive, "Должность: ");
    std::string yearLine = readLineFrom(in, interactive, "Год поступления на работу: ");

    // Строковый поток превращает текст в число и проверяет, что после числа ничего нет
    std::istringstream yearStream(yearLine);
    int year = 0;
    char rest = 0;
    if (!(yearStream >> year) || (yearStream >> rest)) {
        throw AppException("Год поступления должен быть целым числом, а не \"" + yearLine + "\"");
    }
    worker.setAll(fullName, position, year);
    return in;
}
