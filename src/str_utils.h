#pragma once

namespace StrUtils {
inline std::generator<std::string_view> Split(string_view str, char delim, bool skipEmpty = true) {
    size_t start = 0;
    
    while (start < str.size()) {
        size_t end = str.find(delim, start);
        if (end == std::string_view::npos) {
            end = str.size();
        }
        
        std::string_view token = str.substr(start, end - start);
        
        if (!skipEmpty || !token.empty()) {
            co_yield token;
        }
        
        start = end + 1;
    }
    
    // Крайний случай: если строка заканчивается на делитель, 
    // и нам НУЖНО возвращать пустые элементы
    if (!skipEmpty && !str.empty() && str.back() == delim) {
        co_yield "";
    }
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
