#ifndef PERSON_H
#define PERSON_H

#include <iostream>
#include <string>

// Абстрактный базовый класс «Человек»: хранит ФИО.
// Наследники уточняют, кем является человек (например, работник организации).
class Person {
protected:
    std::string fullName;

    // Проверка ФИО: не пустое и без цифр; при ошибке бросает AppException
    static void checkFullName(const std::string& fullName);

public:
    Person();
    explicit Person(const std::string& fullName);
    Person(const Person& other);
    virtual ~Person();

    Person& operator=(const Person& other);

    std::string getFullName() const;
    void setFullName(const std::string& fullName);
    // Фамилия — первое слово ФИО
    std::string getSurname() const;

    // Кем является человек ("Работник", ...)
    virtual std::string getRole() const = 0;
    // Вывод всех данных объекта
    virtual void print(std::ostream& out) const = 0;
};

#endif
