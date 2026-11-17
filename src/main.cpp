#include "container/WorkerList.h"
#include "entities/Worker.h"
#include "exceptions/AppException.h"
#include "input/Input.h"
#include "text/TextDocument.h"

#include <iostream>
#include <new>

#ifdef _WIN32
#include <windows.h>
#endif

using Input::readInt;
using Input::readString;

// ---------- Часть 1. Работники (стандартные потоки) ----------

// Номер работника, выбранный пользователем; возвращает индекс с нуля
static int askWorkerIndex(const WorkerList& list, const std::string& prompt) {
    if (list.isEmpty()) {
        throw AppException("В списке нет ни одного работника");
    }
    return readInt(prompt + " (1-" + std::to_string(list.getCount()) + "): ",
                   1, list.getCount()) - 1;
}

// Ввод нового работника с клавиатуры перегруженным оператором >>
static Worker* inputWorker() {
    Worker* worker = new Worker();
    try {
        std::cin >> *worker;
    } catch (...) {
        delete worker;
        throw;
    }
    return worker;
}

static void addWorkerSorted(WorkerList& list) {
    std::cout << "--- Новый работник (будет поставлен по алфавиту) ---" << std::endl;
    Worker* worker = inputWorker();
    int index = list.addSorted(worker);
    std::cout << "Работник добавлен на позицию " << index + 1 << std::endl;
}

static void addWorkerAt(WorkerList& list) {
    int position = readInt("Позиция для вставки (1-" + std::to_string(list.getCount() + 1) + "): ",
                           1, list.getCount() + 1);
    std::cout << "--- Новый работник ---" << std::endl;
    Worker* worker = inputWorker();
    try {
        list.insertAt(position - 1, worker);
    } catch (...) {
        delete worker;
        throw;
    }
    std::cout << "Работник вставлен на позицию " << position << std::endl;
}

static void editWorker(WorkerList& list) {
    int index = askWorkerIndex(list, "Номер работника для изменения");
    std::cout << list[index] << std::endl;
    std::cout << "1. ФИО\n2. Должность\n3. Год поступления на работу\n"
              << "4. Все поля заново\n0. Отмена" << std::endl;
    int choice = readInt("Какое поле изменить: ", 0, 4);
    if (choice == 0) {
        return;
    }
    // Меняем копию (конструктор копирования); в список она попадает оператором =
    // только если все новые значения прошли проверку
    Worker edited(list[index]);
    switch (choice) {
        case 1: edited.setFullName(readString("Новое ФИО: ")); break;
        case 2: edited.setPosition(readString("Новая должность: ")); break;
        case 3:
            edited.setHireYear(readInt("Новый год поступления: ", -100000, 100000));
            break;
        case 4: std::cin >> edited; break;
    }
    list[index] = edited;
    std::cout << "Изменено:\n" << list[index] << std::endl;
    if (!list.isSorted()) {
        std::cout << "Порядок по алфавиту нарушен — упорядочить можно пунктом 6" << std::endl;
    }
}

static void removeWorker(WorkerList& list) {
    int index = askWorkerIndex(list, "Номер работника для удаления");
    std::string name = list[index].getFullName();
    list.remove(index);
    std::cout << "Работник \"" << name << "\" удалён" << std::endl;
}

static void findExperienced(const WorkerList& list) {
    if (list.isEmpty()) {
        throw AppException("В списке нет ни одного работника");
    }
    std::cout << "Текущий год: " << Worker::currentYear() << std::endl;
    int years = readInt("Показать работников со стажем больше (лет): ", 0, 100);
    list.printExperiencedOver(years, std::cout);
}

// ---------- Часть 2. Текст (файловые и строковые потоки) ----------

static int askSentenceIndex(const TextDocument& text, const std::string& prompt) {
    if (text.isEmpty()) {
        throw AppException("Текст не загружен или в нём нет предложений");
    }
    return readInt(prompt + " (1-" + std::to_string(text.getCount()) + "): ",
                   1, text.getCount()) - 1;
}

static void editSentence(TextDocument& text) {
    int index = askSentenceIndex(text, "Номер предложения для изменения");
    std::cout << "Сейчас: " << text[index] << std::endl;
    text.setSentence(index, readString("Новый текст предложения: "));
    std::cout << "Изменено: [" << index + 1 << "] " << text[index] << std::endl;
}

static void insertSentence(TextDocument& text) {
    int position = readInt("Позиция для вставки (1-" + std::to_string(text.getCount() + 1) + "): ",
                           1, text.getCount() + 1);
    text.insertAt(position - 1, readString("Текст предложения: "));
    std::cout << "Предложение вставлено на позицию " << position << std::endl;
}

static void saveWithoutCommas(const TextDocument& text) {
    if (text.isEmpty()) {
        throw AppException("Текст не загружен или в нём нет предложений");
    }
    std::string fileName = readString("Имя файла для сохранения: ");
    // Копия текста (конструктор копирования), из которой убираются предложения с запятыми
    TextDocument filtered(text);
    int removed = filtered.removeWithCommas();
    if (filtered.isEmpty()) {
        throw AppException("В тексте нет предложений без запятых — сохранять нечего");
    }
    filtered.saveToFile(fileName);
    std::cout << "В файл \"" << fileName << "\" записано предложений: " << filtered.getCount()
              << " (пропущено с запятыми: " << removed << ")" << std::endl;
}

// ---------- Меню ----------

static void printMenu(const WorkerList& list, const TextDocument& text) {
    std::cout << "\n===== Работники: " << list.getTitle() << " (" << list.getCount() << ", "
              << (list.isSorted() ? "по алфавиту" : "порядок нарушен") << ") =====\n"
              << " 1. Показать всех работников\n"
              << " 2. Добавить работника (место по алфавиту)\n"
              << " 3. Добавить работника на заданную позицию\n"
              << " 4. Изменить данные работника\n"
              << " 5. Удалить работника\n"
              << " 6. Упорядочить список по алфавиту\n"
              << " 7. Работники со стажем больше заданного\n"
              << " 8. Сохранить список в файл\n"
              << " 9. Загрузить список из файла\n"
              << "10. Удалить всех работников\n"
              << "11. Изменить название организации\n"
              << "----- Текст: " << text.getFileName() << " (предложений: " << text.getCount()
              << ") -----\n"
              << "12. Загрузить текст из файла\n"
              << "13. Показать все предложения\n"
              << "14. Показать предложения без запятых\n"
              << "15. Изменить предложение\n"
              << "16. Удалить предложение\n"
              << "17. Добавить предложение на заданную позицию\n"
              << "18. Сохранить предложения без запятых в файл\n"
              << " 0. Выход\n";
}

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    WorkerList list;
    TextDocument text;
    bool running = true;

    while (running) {
        try {
            printMenu(list, text);
            switch (readInt("Выберите пункт: ", 0, 18)) {
                case 1: std::cout << list; break;
                case 2: addWorkerSorted(list); break;
                case 3: addWorkerAt(list); break;
                case 4: editWorker(list); break;
                case 5: removeWorker(list); break;
                case 6:
                    list.sortByName();
                    std::cout << "Список упорядочен по алфавиту" << std::endl;
                    break;
                case 7: findExperienced(list); break;
                case 8: {
                    std::string fileName = readString("Имя файла для сохранения: ");
                    list.saveToFile(fileName);
                    std::cout << "Список сохранён в файл \"" << fileName << "\"" << std::endl;
                    break;
                }
                case 9: {
                    std::string fileName = readString("Имя файла для загрузки: ");
                    list.loadFromFile(fileName);
                    std::cout << "Загружено работников: " << list.getCount() << std::endl;
                    break;
                }
                case 10:
                    list.clear();
                    std::cout << "Все работники удалены" << std::endl;
                    break;
                case 11: list.setTitle(readString("Новое название организации: ")); break;
                case 12: {
                    std::string fileName = readString("Имя текстового файла: ");
                    text.loadFromFile(fileName);
                    std::cout << "Прочитано предложений: " << text.getCount() << std::endl;
                    break;
                }
                case 13: std::cout << text; break;
                case 14:
                    if (text.isEmpty()) {
                        throw AppException("Текст не загружен или в нём нет предложений");
                    }
                    text.printWithoutCommas(std::cout);
                    break;
                case 15: editSentence(text); break;
                case 16: {
                    int index = askSentenceIndex(text, "Номер предложения для удаления");
                    text.remove(index);
                    std::cout << "Предложение удалено" << std::endl;
                    break;
                }
                case 17: insertSentence(text); break;
                case 18: saveWithoutCommas(text); break;
                case 0: running = false; break;
            }
        } catch (const AppException& e) {
            if (std::cin.eof()) {
                running = false;
            } else {
                std::cout << "Ошибка: " << e.what() << std::endl;
            }
        } catch (const std::bad_alloc&) {
            std::cout << "Ошибка: недостаточно памяти" << std::endl;
        }
    }

    std::cout << "Работа программы завершена" << std::endl;
    return 0;
}
