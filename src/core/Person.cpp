#include "Person.h"
#include "../exceptions/AppException.h"
#include "../utils/TextUtils.h"

Person::Person() : fullName("Не указано") {
    std::cout << "[Person] Вызван конструктор без параметров" << std::endl;
}

Person::Person(const std::string& fullName) : fullName("Не указано") {
    setFullName(fullName);
    std::cout << "[Person] Вызван конструктор с параметрами (" << this->fullName << ")" << std::endl;
}

Person::Person(const Person& other) : fullName(other.fullName) {
    std::cout << "[Person] Вызван конструктор копирования (" << fullName << ")" << std::endl;
}

Person::~Person() {
    std::cout << "[Person] Вызван деструктор (" << fullName << ")" << std::endl;
}

Person& Person::operator=(const Person& other) {
    if (this != &other) {
        fullName = other.fullName;
    }
    return *this;
}

std::string Person::getFullName() const { return fullName; }

void Person::setFullName(const std::string& fullName) {
    std::string value = TextUtils::trim(fullName);
    checkFullName(value);
    this->fullName = value;
}

std::string Person::getSurname() const {
    return TextUtils::firstWord(fullName);
}

void Person::checkFullName(const std::string& fullName) {
    if (TextUtils::isBlank(fullName)) {
        throw AppException("ФИО не может быть пустым");
    }
    for (char c : fullName) {
        if (c >= '0' && c <= '9') {
            throw AppException("ФИО не может содержать цифры: \"" + fullName + "\"");
        }
    }
}
