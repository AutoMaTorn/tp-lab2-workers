#include "WorkerList.h"
#include "../exceptions/AppException.h"
#include "../utils/TextUtils.h"

#include <fstream>
#include <sstream>
#include <utility>

static const char* FILE_HEADER = "WORKER_LIST";
static const int INITIAL_CAPACITY = 4;

WorkerList::WorkerList()
    : title("Организация"), items(new Worker*[INITIAL_CAPACITY]), count(0),
      capacity(INITIAL_CAPACITY) {
    std::cout << "[WorkerList] Вызван конструктор без параметров" << std::endl;
}

WorkerList::WorkerList(const std::string& title)
    : title("Организация"), items(new Worker*[INITIAL_CAPACITY]), count(0),
      capacity(INITIAL_CAPACITY) {
    try {
        setTitle(title);
    } catch (...) {
        delete[] items;
        throw;
    }
    std::cout << "[WorkerList] Вызван конструктор с параметрами (" << this->title << ")" << std::endl;
}

WorkerList::WorkerList(const WorkerList& other)
    : title(other.title), items(new Worker*[other.capacity]), count(0),
      capacity(other.capacity) {
    // Глубокое копирование: у каждого списка свои экземпляры работников
    try {
        for (int i = 0; i < other.count; i++) {
            items[i] = new Worker(*other.items[i]);
            count++;
        }
    } catch (...) {
        clear();
        delete[] items;
        throw;
    }
    std::cout << "[WorkerList] Вызван конструктор копирования" << std::endl;
}

WorkerList::~WorkerList() {
    std::cout << "[WorkerList] Вызван деструктор (" << title << ")" << std::endl;
    clear();
    delete[] items;
}

WorkerList& WorkerList::operator=(const WorkerList& other) {
    if (this != &other) {
        WorkerList copy(other);
        // Старые данные окажутся в copy и удалятся её деструктором
        swapWith(copy);
    }
    return *this;
}

WorkerList& WorkerList::operator+=(Worker* worker) {
    addSorted(worker);
    return *this;
}

Worker& WorkerList::operator[](int index) const {
    checkIndex(index);
    return *items[index];
}

void WorkerList::swapWith(WorkerList& other) {
    std::swap(title, other.title);
    std::swap(items, other.items);
    std::swap(count, other.count);
    std::swap(capacity, other.capacity);
}

std::string WorkerList::getTitle() const { return title; }

void WorkerList::setTitle(const std::string& title) {
    if (TextUtils::isBlank(title)) {
        throw AppException("Название организации не может быть пустым");
    }
    this->title = TextUtils::trim(title);
}

int WorkerList::getCount() const { return count; }
bool WorkerList::isEmpty() const { return count == 0; }

bool WorkerList::isSorted() const {
    for (int i = 1; i < count; i++) {
        if (*items[i] < *items[i - 1]) return false;
    }
    return true;
}

void WorkerList::grow() {
    int newCapacity = capacity * 2;
    Worker** newItems = new Worker*[newCapacity];
    for (int i = 0; i < count; i++) {
        newItems[i] = items[i];
    }
    delete[] items;
    items = newItems;
    capacity = newCapacity;
}

void WorkerList::checkIndex(int index) const {
    if (index < 0 || index >= count) {
        throw AppException("Работника с номером " + std::to_string(index + 1) +
                           " нет (всего работников: " + std::to_string(count) + ")");
    }
}

void WorkerList::insertAt(int index, Worker* worker) {
    if (worker == nullptr) {
        throw AppException("Нельзя добавить пустой объект");
    }
    if (index < 0 || index > count) {
        throw AppException("Нельзя вставить на позицию " + std::to_string(index + 1) +
                           " (допустимо от 1 до " + std::to_string(count + 1) + ")");
    }
    if (count == capacity) {
        grow();
    }
    for (int i = count; i > index; i--) {
        items[i] = items[i - 1];
    }
    items[index] = worker;
    count++;
}

int WorkerList::addSorted(Worker* worker) {
    if (worker == nullptr) {
        throw AppException("Нельзя добавить пустой объект");
    }
    // Ищем первого работника, который по алфавиту стоит после нового
    int index = 0;
    while (index < count && !(*worker < *items[index])) {
        index++;
    }
    insertAt(index, worker);
    return index;
}

void WorkerList::remove(int index) {
    checkIndex(index);
    delete items[index];
    for (int i = index; i < count - 1; i++) {
        items[i] = items[i + 1];
    }
    count--;
}

void WorkerList::clear() {
    for (int i = 0; i < count; i++) {
        delete items[i];
    }
    count = 0;
}

void WorkerList::sortByName() {
    // Сортировка вставками: переставляются только указатели, объекты не копируются
    for (int i = 1; i < count; i++) {
        Worker* current = items[i];
        int j = i - 1;
        while (j >= 0 && *current < *items[j]) {
            items[j + 1] = items[j];
            j--;
        }
        items[j + 1] = current;
    }
}

int WorkerList::printExperiencedOver(int years, std::ostream& out) const {
    int found = 0;
    for (int i = 0; i < count; i++) {
        int experience = items[i]->getExperience();
        if (experience > years) {
            if (found == 0) {
                out << "Работники со стажем больше " << years << " "
                    << TextUtils::yearsWord(years) << ":\n";
            }
            found++;
            out << "  " << found << ". " << TextUtils::padRight(items[i]->getSurname(), 20)
                << "стаж " << experience << " " << TextUtils::yearsWord(experience) << "\n";
        }
    }
    if (found == 0) {
        out << "Работников со стажем больше " << years << " "
            << TextUtils::yearsWord(years) << " нет\n";
    }
    return found;
}

void WorkerList::print(std::ostream& out) const {
    out << "=== " << title << " (работников: " << count << ") ===\n";
    if (count == 0) {
        out << "    Список пуст\n";
        return;
    }
    out << TextUtils::padRight(" №", 4) << TextUtils::padRight("ФИО", 30)
        << TextUtils::padRight("Должность", 26) << TextUtils::padRight("Год", 6) << "Стаж\n";
    for (int i = 0; i < count; i++) {
        std::ostringstream number;
        number << " " << i + 1;
        out << TextUtils::padRight(number.str(), 4)
            << TextUtils::padRight(items[i]->getFullName(), 30)
            << TextUtils::padRight(items[i]->getPosition(), 26)
            << TextUtils::padRight(std::to_string(items[i]->getHireYear()), 6)
            << items[i]->getExperience() << "\n";
    }
    if (!isSorted()) {
        out << "    (порядок не алфавитный — упорядочить можно пунктом 6)\n";
    }
}

void WorkerList::saveToFile(const std::string& fileName) const {
    std::ofstream out(fileName);
    if (!out.is_open()) {
        throw AppException("Не удалось открыть файл \"" + fileName + "\" для записи");
    }
    out << FILE_HEADER << "\n" << title << "\n" << count << "\n";
    for (int i = 0; i < count; i++) {
        items[i]->save(out);
    }
    if (!out) {
        throw AppException("Ошибка записи в файл \"" + fileName + "\"");
    }
}

// Чтение строки без завершающего '\r' (если файл правили в Windows)
static std::string readLine(std::istream& in) {
    std::string line;
    if (!std::getline(in, line)) {
        throw AppException("Ошибка чтения файла: файл обрывается");
    }
    if (!line.empty() && line[line.size() - 1] == '\r') {
        line.erase(line.size() - 1);
    }
    return line;
}

void WorkerList::loadFromFile(const std::string& fileName) {
    std::ifstream in(fileName);
    if (!in.is_open()) {
        throw AppException("Не удалось открыть файл \"" + fileName + "\" для чтения");
    }
    std::string header = readLine(in);
    // Файл мог быть сохранён в Блокноте с меткой BOM в начале
    if (header.size() >= 3 && header.compare(0, 3, "\xEF\xBB\xBF") == 0) {
        header.erase(0, 3);
    }
    if (header != FILE_HEADER) {
        throw AppException("Файл \"" + fileName + "\" не является файлом списка работников");
    }

    // Читаем во временный список: при ошибке текущие данные не пострадают
    WorkerList loaded(readLine(in));
    std::string countLine = readLine(in);
    std::istringstream countStream(countLine);
    int total = 0;
    char rest = 0;
    if (!(countStream >> total) || (countStream >> rest) || total < 0) {
        throw AppException("Ошибка чтения файла: неверное количество работников \"" +
                           countLine + "\"");
    }
    for (int i = 0; i < total; i++) {
        Worker* worker = new Worker();
        try {
            in >> *worker;   // тот же оператор извлечения, что и для клавиатуры
            loaded.insertAt(loaded.getCount(), worker);
        } catch (...) {
            delete worker;
            throw;
        }
    }
    swapWith(loaded);
}

std::ostream& operator<<(std::ostream& out, const WorkerList& list) {
    list.print(out);
    return out;
}
