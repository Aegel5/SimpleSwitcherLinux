#pragma once



class HotKey {
    vector<ScanCode> codes;
    bool m_isUp = false;
    bool m_compareAllowSides = false;
    static inline bool Compare(ScanCode c1, ScanCode c2, bool allowSides) { 
        if(!allowSides)
            return c1 == c2;
        return to_left(c1) == to_left(c2);
     }

   public:
    static inline ScanCode to_left(ScanCode code){
        if(code == KEY_RIGHTSHIFT) return KEY_LEFTSHIFT;
        if(code == KEY_RIGHTCTRL) return KEY_LEFTCTRL;
        if(code == KEY_RIGHTMETA) return KEY_LEFTMETA;
        if(code == KEY_RIGHTALT) return KEY_LEFTALT;
        return code;
    }   
   void pop_back() {codes.pop_back();}
   bool empty() const {return codes.empty();}
   auto begin(this auto&& self){return self.codes.begin();}
   auto end(this auto&& self){return self.codes.end();}
   int size() const {return codes.size();}
   HotKey() = default;
   explicit HotKey(string_view s){FromString(s);}
   explicit HotKey(ScanCode code) {Add(code);}
   explicit HotKey(ScanCode c1, ScanCode c2) {Add(c1); Add(c2);}
   void clear(){
    *this = {};
   }
   ScanCode Key() {return codes.empty()?0:codes.back();}
   void SetUp(bool val = true){m_isUp = val;}
   void Set_is_common(bool val = true){m_compareAllowSides = val;}
   bool is_common() const {return m_compareAllowSides;}
    void Add(ScanCode code) {
        auto it = std::find(codes.begin(), codes.end(), code);
        if (it == codes.end()) {
            codes.push_back(code);
        }
    }
    static bool IsMod(ScanCode code){
        return is_in(to_left(code), KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_LEFTMETA, KEY_LEFTALT);
    }
    void Remove(ScanCode code) {
        auto it = std::find(codes.begin(), codes.end(), code);
        if (it != codes.end()) {
            codes.erase(it);
        }
    }
    bool HasKey(ScanCode code, bool allowSides) const{
        for(auto c : codes){
            if(Compare(c, code, allowSides)) return true;
        }
        return false;
    }
    bool Compare(const HotKey& other) const {
        // 1. Базовые проверки
        if (m_isUp != other.m_isUp)
            return false;
        if (codes.size() != other.codes.size())
            return false;

        // Если оба списка пустые — они равны
        if (codes.empty())
            return true;

        // Определяем режим сравнения клавиш
        bool allowSides = this->m_compareAllowSides || other.m_compareAllowSides;

        if (!Compare(codes.back(), other.codes.back(), allowSides))
            return false;

        // Если в хоткее была всего одна клавиша, и она совпала
        if (codes.size() == 1)
            return true;

        // 3. Сравнение оставшейся части (без учета порядка)
        if (!allowSides) {
            // Строгое совпадение без учета порядка для n-1 элементов
            return std::is_permutation(codes.begin(), codes.end() - 1, other.codes.begin());
        }

        // Массив флагов на стеке, чтобы помнить, какие клавиши из other уже "заняты"
        // Задаем лимит, например, 32 клавиши (для хоткеев этого более чем достаточно)
        constexpr size_t MAX_HOTKEY_SIZE = 16;

        // Если клавиш больше лимита, можно временно упасть в false или увеличить константу
        if (codes.size() > MAX_HOTKEY_SIZE)
            return false;

        bool used[MAX_HOTKEY_SIZE] = {false};
        const size_t remainingCount = codes.size() - 1;

        // Перебираем элементы текущего хоткея (кроме последнего)
        for (size_t i = 0; i < remainingCount; ++i) {
            bool foundMatch = false;

            // Ищем совпадение среди элементов 'other' (кроме последнего)
            for (size_t j = 0; j < remainingCount; ++j) {
                // Если этот элемент other еще не был сопоставлен и коды эквивалентны
                if (!used[j] && Compare(codes[i], other.codes[j], true)) {
                    used[j] = true;  // Помечаем как использованный
                    foundMatch = true;
                    break;
                }
            }

            // Если для текущей клавиши не нашлось свободной пары в other
            if (!foundMatch)
                return false;
        }

        return true;
    }
    void FromString(string_view str);
    string ToString() const;
    static string CodeToString(ScanCode code, bool common = false);
};