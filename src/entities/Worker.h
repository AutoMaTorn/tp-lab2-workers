#ifndef WORKER_H
#define WORKER_H

#include "../core/Person.h"

// Работник (WORKER): ФИО (из Person), название должности, год поступления на работу.
class Worker : public Person {
private:
    std::string position;
    int hireYear;

    static void checkPosition(const std::string& position);
    static void checkHireYear(int hireYear);

public:
    // Самый ранний допустимый год поступления на работу
    static const int MIN_HIRE_YEAR = 1950;

    Worker();
    Worker(const std::string& fullName, const std::string& position, int hireYear);
    Worker(const Worker& other);
    ~Worker() override;

    Worker& operator=(const Worker& other);
    bool operator==(const Worker& other) const;
    // Сравнение по алфавиту (по ФИО) — используется для упорядочивания списка
    bool operator<(const Worker& other) const;

    std::string getPosition() const;
    int getHireYear() const;
    void setPosition(const std::string& position);
    void setHireYear(int hireYear);
    // Установка всех полей сразу: сначала проверяются все значения, потом присваиваются
    void setAll(const std::string& fullName, const std::string& position, int hireYear);

    // Стаж работы в организации (полных лет на текущий год)
    int getExperience() const;
    // Текущий год по системным часам компьютера
    static int currentYear();

    std::string getRole() const override;
    void print(std::ostream& out) const override;

    // Запись полей в файл (по одному значению на строку, в формате, который читает operator>>)
    void save(std::ostream& out) const;

    // Ввод работника: с клавиатуры (с подсказками) или из файла
    friend std::istream& operator>>(std::istream& in, Worker& worker);
};

// Вывод работника в поток
std::ostream& operator<<(std::ostream& out, const Worker& worker);
std::istream& operator>>(std::istream& in, Worker& worker);

#endif
