#include "TextDocument.h"
#include "../exceptions/AppException.h"
#include "../utils/TextUtils.h"

#include <fstream>
#include <sstream>
#include <utility>

static const int INITIAL_CAPACITY = 4;

TextDocument::TextDocument()
    : fileName("(файл не загружен)"), sentences(new std::string[INITIAL_CAPACITY]), count(0),
      capacity(INITIAL_CAPACITY) {
    std::cout << "[TextDocument] Вызван конструктор без параметров" << std::endl;
}

TextDocument::TextDocument(const std::string& fileName)
    : fileName("(файл не загружен)"), sentences(new std::string[INITIAL_CAPACITY]), count(0),
      capacity(INITIAL_CAPACITY) {
    try {
        loadFromFile(fileName);
    } catch (...) {
        delete[] sentences;
        throw;
    }
    std::cout << "[TextDocument] Вызван конструктор с параметрами (" << this->fileName << ")"
              << std::endl;
}

TextDocument::TextDocument(const TextDocument& other)
    : fileName(other.fileName), sentences(new std::string[other.capacity]), count(other.count),
      capacity(other.capacity) {
    for (int i = 0; i < count; i++) {
        sentences[i] = other.sentences[i];
    }
    std::cout << "[TextDocument] Вызван конструктор копирования" << std::endl;
}

TextDocument::~TextDocument() {
    std::cout << "[TextDocument] Вызван деструктор (" << fileName << ")" << std::endl;
    delete[] sentences;
}

TextDocument& TextDocument::operator=(const TextDocument& other) {
    if (this != &other) {
        TextDocument copy(other);
        swapWith(copy);
    }
    return *this;
}

const std::string& TextDocument::operator[](int index) const {
    checkIndex(index);
    return sentences[index];
}

void TextDocument::swapWith(TextDocument& other) {
    std::swap(fileName, other.fileName);
    std::swap(sentences, other.sentences);
    std::swap(count, other.count);
    std::swap(capacity, other.capacity);
}

std::string TextDocument::getFileName() const { return fileName; }
int TextDocument::getCount() const { return count; }
bool TextDocument::isEmpty() const { return count == 0; }

std::string TextDocument::getSentence(int index) const {
    checkIndex(index);
    return sentences[index];
}

void TextDocument::setSentence(int index, const std::string& sentence) {
    checkIndex(index);
    sentences[index] = normalize(sentence);
}

void TextDocument::grow() {
    int newCapacity = capacity * 2;
    std::string* newSentences = new std::string[newCapacity];
    for (int i = 0; i < count; i++) {
        newSentences[i].swap(sentences[i]);   // строки не копируются, а обмениваются
    }
    delete[] sentences;
    sentences = newSentences;
    capacity = newCapacity;
}

void TextDocument::checkIndex(int index) const {
    if (index < 0 || index >= count) {
        throw AppException("Предложения с номером " + std::to_string(index + 1) +
                           " нет (всего предложений: " + std::to_string(count) + ")");
    }
}

std::string TextDocument::normalize(const std::string& sentence) {
    // Строковый поток читает слова, пропуская любые пробелы и переводы строк,
    // а другой строковый поток собирает их обратно через один пробел
    std::istringstream in(sentence);
    std::ostringstream out;
    std::string word;
    bool first = true;
    while (in >> word) {
        if (!first) out << ' ';
        out << word;
        first = false;
    }
    std::string result = out.str();
    if (result.empty()) {
        throw AppException("Предложение не может быть пустым");
    }
    return result;
}

// Символ завершает предложение: . ! ? (многоточие «…» проверяется отдельно)
static bool isTerminator(int c) {
    return c == '.' || c == '!' || c == '?';
}

static bool isSpaceChar(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

static bool endsWith(const std::string& s, const std::string& suffix) {
    return s.size() >= suffix.size() &&
           s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void TextDocument::splitIntoSentences(const std::string& text) {
    static const std::string ELLIPSIS = "\xE2\x80\xA6";       // …
    static const std::string CLOSE_QUOTE = "\xC2\xBB";        // »

    std::istringstream in(text);
    std::string current;
    char c;
    while (in.get(c)) {
        current += c;
        if (!isTerminator(c) && !endsWith(current, ELLIPSIS)) {
            continue;
        }
        // Забираем идущие подряд знаки конца (?!, ...) и закрывающие кавычки/скобки
        while (true) {
            int next = in.peek();
            if (isTerminator(next) || next == '"' || next == '\'' || next == ')') {
                current += static_cast<char>(in.get());
            } else if (next == 0xC2 || next == 0xE2) {
                // Возможно, это закрывающая «ёлочка» » или многоточие … (несколько байт UTF-8)
                std::string tail(1, static_cast<char>(in.get()));
                while ((in.peek() & 0xC0) == 0x80) {
                    tail += static_cast<char>(in.get());
                }
                if (tail == CLOSE_QUOTE || tail == ELLIPSIS) {
                    current += tail;
                } else {
                    // Не наш символ — возвращаем байты обратно в поток
                    for (std::string::size_type k = 0; k < tail.size(); k++) in.unget();
                    break;
                }
            } else {
                break;
            }
        }
        // Конец предложения — только если дальше пробел или конец текста
        // (так «3.14» и «example.com» не разрезаются)
        int next = in.peek();
        if (next == EOF || isSpaceChar(next)) {
            if (!TextUtils::isBlank(current)) {
                insertAt(count, current);
            }
            current.clear();
        }
    }
    // Хвост без знака препинания в конце тоже считаем предложением
    if (!TextUtils::isBlank(current)) {
        insertAt(count, current);
    }
}

void TextDocument::loadFromFile(const std::string& fileName) {
    if (TextUtils::isBlank(fileName)) {
        throw AppException("Имя файла не может быть пустым");
    }
    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw AppException("Не удалось открыть файл \"" + fileName + "\" для чтения");
    }
    // Весь файл переносится в строковый поток одной операцией
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();
    if (text.size() >= 3 && text.compare(0, 3, "\xEF\xBB\xBF") == 0) {
        text.erase(0, 3);   // метка BOM от Блокнота
    }
    if (TextUtils::isBlank(text)) {
        throw AppException("Файл \"" + fileName + "\" пуст — в нём нет ни одного предложения");
    }

    // Разбираем во временный объект: при ошибке текущий текст не пострадает
    TextDocument loaded;
    loaded.splitIntoSentences(text);
    loaded.fileName = fileName;
    swapWith(loaded);
}

void TextDocument::saveToFile(const std::string& fileName) const {
    if (TextUtils::isBlank(fileName)) {
        throw AppException("Имя файла не может быть пустым");
    }
    std::ofstream out(fileName);
    if (!out.is_open()) {
        throw AppException("Не удалось открыть файл \"" + fileName + "\" для записи");
    }
    for (int i = 0; i < count; i++) {
        out << sentences[i] << "\n";
    }
    if (!out) {
        throw AppException("Ошибка записи в файл \"" + fileName + "\"");
    }
}

void TextDocument::insertAt(int index, const std::string& sentence) {
    if (index < 0 || index > count) {
        throw AppException("Нельзя вставить на позицию " + std::to_string(index + 1) +
                           " (допустимо от 1 до " + std::to_string(count + 1) + ")");
    }
    std::string value = normalize(sentence);
    if (count == capacity) {
        grow();
    }
    for (int i = count; i > index; i--) {
        sentences[i].swap(sentences[i - 1]);
    }
    sentences[index] = value;
    count++;
}

void TextDocument::remove(int index) {
    checkIndex(index);
    for (int i = index; i < count - 1; i++) {
        sentences[i].swap(sentences[i + 1]);
    }
    sentences[count - 1].clear();
    count--;
}

void TextDocument::clear() {
    for (int i = 0; i < count; i++) {
        sentences[i].clear();
    }
    count = 0;
}

bool TextDocument::hasComma(const std::string& sentence) {
    return sentence.find(',') != std::string::npos;
}

int TextDocument::removeWithCommas() {
    int removed = 0;
    int i = 0;
    while (i < count) {
        if (hasComma(sentences[i])) {
            remove(i);
            removed++;
        } else {
            i++;
        }
    }
    return removed;
}

int TextDocument::printWithoutCommas(std::ostream& out) const {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (!hasComma(sentences[i])) {
            if (found == 0) {
                out << "Предложения без запятых (номер — позиция в тексте):\n";
            }
            found++;
            out << "  [" << i + 1 << "] " << sentences[i] << "\n";
        }
    }
    if (found == 0) {
        out << "В тексте нет предложений без запятых\n";
    } else {
        out << "Найдено: " << found << " из " << count << "\n";
    }
    return found;
}

void TextDocument::print(std::ostream& out) const {
    out << "=== Текст: " << fileName << " (предложений: " << count << ") ===\n";
    if (count == 0) {
        out << "    Текст пуст\n";
        return;
    }
    for (int i = 0; i < count; i++) {
        out << "  [" << i + 1 << "] " << sentences[i] << "\n";
    }
}

std::ostream& operator<<(std::ostream& out, const TextDocument& text) {
    text.print(out);
    return out;
}
