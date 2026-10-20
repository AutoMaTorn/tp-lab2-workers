#include "TextUtils.h"

#include <sstream>

namespace TextUtils {

static bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

bool isBlank(const std::string& s) {
    for (char c : s) {
        if (!isSpace(c)) return false;
    }
    return true;
}

std::string trim(const std::string& s) {
    std::string::size_type begin = 0;
    std::string::size_type end = s.size();
    while (begin < end && isSpace(s[begin])) begin++;
    while (end > begin && isSpace(s[end - 1])) end--;
    return s.substr(begin, end - begin);
}

int length(const std::string& s) {
    int n = 0;
    for (char c : s) {
        // Байты вида 10xxxxxx — продолжение многобайтового символа
        if ((static_cast<unsigned char>(c) & 0xC0) != 0x80) n++;
    }
    return n;
}

std::string padRight(const std::string& s, int width) {
    int len = length(s);
    if (len >= width) return s + " ";
    return s + std::string(width - len, ' ');
}

// Чтение одного символа UTF-8 начиная с позиции pos; pos сдвигается на следующий символ
static unsigned int nextCodePoint(const std::string& s, std::string::size_type& pos) {
    unsigned char c = static_cast<unsigned char>(s[pos++]);
    if (c < 0x80) return c;
    int extra = 0;
    unsigned int cp = 0;
    if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; extra = 1; }
    else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; extra = 2; }
    else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; extra = 3; }
    else return c;   // некорректный байт — сравниваем как есть
    while (extra-- > 0 && pos < s.size()) {
        cp = (cp << 6) | (static_cast<unsigned char>(s[pos++]) & 0x3F);
    }
    return cp;
}

// Ключ сортировки символа: строчные и прописные равны, ё стоит сразу после е
static unsigned int sortKey(unsigned int cp) {
    if (cp >= 'A' && cp <= 'Z') cp += 'a' - 'A';
    if (cp >= 0x0410 && cp <= 0x042F) cp += 0x20;   // А..Я -> а..я
    if (cp == 0x0401) cp = 0x0451;                   // Ё -> ё
    if (cp == 0x0451) return 0x0435 * 2 + 1;         // ё между е и ж
    return cp * 2;
}

int compareAlpha(const std::string& a, const std::string& b) {
    std::string::size_type i = 0;
    std::string::size_type j = 0;
    while (i < a.size() && j < b.size()) {
        unsigned int ka = sortKey(nextCodePoint(a, i));
        unsigned int kb = sortKey(nextCodePoint(b, j));
        if (ka != kb) return ka < kb ? -1 : 1;
    }
    if (i < a.size()) return 1;
    if (j < b.size()) return -1;
    return 0;
}

std::string firstWord(const std::string& s) {
    // Строковый поток сам пропускает пробелы и читает до следующего пробела
    std::istringstream in(s);
    std::string word;
    in >> word;
    return word;
}

std::string yearsWord(int n) {
    int lastTwo = n % 100;
    int last = n % 10;
    if (lastTwo >= 11 && lastTwo <= 14) return "лет";
    if (last == 1) return "год";
    if (last >= 2 && last <= 4) return "года";
    return "лет";
}

}
