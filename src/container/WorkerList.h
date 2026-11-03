#ifndef WORKER_LIST_H
#define WORKER_LIST_H

#include "../entities/Worker.h"

// Список работников организации: динамический массив указателей на Worker.
// Число работников заранее не известно — массив расширяется по мере добавления.
// Список владеет объектами (удаляет их сам).
class WorkerList {
private:
    std::string title;   // название организации
    Worker** items;
    int count;
    int capacity;

    void grow();
    void checkIndex(int index) const;
    void swapWith(WorkerList& other);

public:
    WorkerList();
    explicit WorkerList(const std::string& title);
    WorkerList(const WorkerList& other);
    ~WorkerList();

    WorkerList& operator=(const WorkerList& other);
    WorkerList& operator+=(Worker* worker);   // добавить с сохранением алфавитного порядка
    Worker& operator[](int index) const;      // доступ по номеру с проверкой

    std::string getTitle() const;
    void setTitle(const std::string& title);
    int getCount() const;
    bool isEmpty() const;
    // Упорядочен ли список по алфавиту
    bool isSorted() const;

    // Вставка на позицию index (0..count); остальные сдвигаются вправо
    void insertAt(int index, Worker* worker);
    // Вставка на место по алфавиту; возвращает индекс, куда вставлен работник
    int addSorted(Worker* worker);
    void remove(int index);
    void clear();
    // Упорядочивание по алфавиту (сортировка вставками)
    void sortByName();

    // Вывод фамилий работников со стажем больше years; возвращает их количество.
    // Если таких нет — выводит сообщение об этом.
    int printExperiencedOver(int years, std::ostream& out) const;
    // Вывод всего списка таблицей
    void print(std::ostream& out) const;

    void saveToFile(const std::string& fileName) const;
    void loadFromFile(const std::string& fileName);
};

std::ostream& operator<<(std::ostream& out, const WorkerList& list);

#endif
