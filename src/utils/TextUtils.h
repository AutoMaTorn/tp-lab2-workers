#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <string>

// Вспомогательные функции для работы со строками в кодировке UTF-8.
namespace TextUtils {
    // Строка пустая или состоит только из пробельных символов
    bool isBlank(const std::string& s);
    // Убрать пробельные символы в начале и в конце строки
    std::string trim(const std::string& s);
    // Количество символов (а не байт) в UTF-8 строке
    int length(const std::string& s);
    // Дополнить строку пробелами справа до ширины width символов
    std::string padRight(const std::string& s, int width);
    // Сравнение строк по алфавиту без учёта регистра (русские и латинские буквы, ё после е).
    // Возвращает число < 0, 0 или > 0, как strcmp.
    int compareAlpha(const std::string& a, const std::string& b);
    // Первое слово строки (для ФИО — фамилия)
    std::string firstWord(const std::string& s);
    // Правильная форма слова: 1 год, 2 года, 5 лет
    std::string yearsWord(int n);
}

#endif
