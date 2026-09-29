#pragma once

namespace StrUtils {
inline auto Split(string_view str, char delim, bool skipEmpty = true) {
    std::vector<std::string> res;
    std::ispanstream data(str);

    std::string line;
    while (std::getline(data, line, delim)) {
        if (skipEmpty && line.empty())
            continue;
        res.push_back(line);
    }

    return res;
}

inline char ToLowerEnglishQuick(char c) {  return (c >= 'A' && c <= 'Z') ? c+32 : c;}

inline void ToLowerEnglishQuick(std::string& str) {
    for (auto& c : str) {
        c = ToLowerEnglishQuick(c);
    }
}

inline bool replaceAll(std::string& s, string_view search, string_view replace) {
    bool found = false;
    size_t pos = 0;
    while ((pos = s.find(search, pos)) != std::string::npos) {
        found = true;
        s.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return found;
}

inline void Trim(std::string& str) {
    const char* whitespaces = " \t\n\r\f\v";  // Все стандартные пробельные символы

    // 1. Убираем с конца
    size_t last = str.find_last_not_of(whitespaces);
    if (last == std::string::npos) {
        str.clear();  // Строка состоит только из пробелов
        return;
    }
    str.erase(last + 1);

    // 2. Убираем с начала
    size_t first = str.find_first_not_of(whitespaces);
    if (first != 0) {
        str.erase(0, first);
    }
}

inline bool TryStringToInt(string_view str, auto& result) {
    if (str.empty()) return false;

    // std::from_chars требует указатели на начало и конец строки
    const char* start = str.data();
    const char* end = start + str.size();

    // Преобразуем строку в число (в десятичной системе)
    auto [ptr, ec] = std::from_chars(start, end, result);

    // Успех, только если нет ошибок и вся строка была прочитана полностью
    return ec == std::errc() && ptr == end;
}

}  // namespace StrUtils
