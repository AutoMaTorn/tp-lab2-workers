#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include <iostream>
#include <string>

// Текст, прочитанный из файла и разбитый на предложения.
// Предложения хранятся в динамическом массиве строк std::string*.
// Чтение файла — файловым потоком (ifstream), разбор на предложения — строковыми потоками.
class TextDocument {
private:
    std::string fileName;      // из какого файла прочитан текст
    std::string* sentences;
    int count;
    int capacity;

    void grow();
    void checkIndex(int index) const;
    void swapWith(TextDocument& other);
    // Разбить текст на предложения и добавить их в конец массива
    void splitIntoSentences(const std::string& text);
    // Привести предложение к общему виду: убрать лишние пробелы, проверить, что не пустое
    static std::string normalize(const std::string& sentence);

public:
    TextDocument();
    explicit TextDocument(const std::string& fileName);   // сразу читает файл
    TextDocument(const TextDocument& other);
    ~TextDocument();

    TextDocument& operator=(const TextDocument& other);
    const std::string& operator[](int index) const;      // предложение по номеру с проверкой

    std::string getFileName() const;
    int getCount() const;
    bool isEmpty() const;
    std::string getSentence(int index) const;
    void setSentence(int index, const std::string& sentence);

    // Прочитать текст из файла (старый текст заменяется только при успешном чтении)
    void loadFromFile(const std::string& fileName);
    // Записать предложения в файл, каждое с новой строки
    void saveToFile(const std::string& fileName) const;

    void insertAt(int index, const std::string& sentence);
    void remove(int index);
    void clear();

    // Есть ли в предложении запятая
    static bool hasComma(const std::string& sentence);
    // Удалить все предложения с запятыми; возвращает, сколько удалено
    int removeWithCommas();
    // Вывести предложения без запятых; если таких нет — сообщение. Возвращает их количество
    int printWithoutCommas(std::ostream& out) const;
    // Вывести все предложения с номерами
    void print(std::ostream& out) const;
};

std::ostream& operator<<(std::ostream& out, const TextDocument& text);

#endif
