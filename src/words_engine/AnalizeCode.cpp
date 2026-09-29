#include "common.h"

extern std::generator<wchar_t> ResolveForAllLayout(ScanCode code, bool is_down);

KEY_TYPE_ANALIZE AnalizeCode(ScanCode code, bool is_shift) {
    switch (code) {
        case KEY_ENTER:
        case KEY_END:
        case KEY_HOME:
        case KEY_NEXT:
        case KEY_DELETE:
        case KEY_ESC:
        case KEY_PAGEUP:
        case KEY_PAGEDOWN:
            return KEY_TYPE_ANALIZE::NO_SYMBOL_CLEAR;

        case KEY_CAPSLOCK:
        case KEY_SCROLLLOCK:
        case KEY_PRINT:
        case KEY_NUMLOCK:
        case KEY_INSERT:
            return KEY_TYPE_ANALIZE::NO_SYMBOL_NO_CLEAR;

        case KEY_SPACE:
            return KEY_TYPE_ANALIZE::SPACE;

        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_DOWN:
            return KEY_TYPE_ANALIZE::NO_SYMBOL_CLEAR;
    }

    if (code >= KEY_F1 && code <= KEY_F12) {
        return KEY_TYPE_ANALIZE::NO_SYMBOL_CLEAR;
    }

    static vector<wchar_t> symbols;
    symbols.clear();

    for (auto smb : ResolveForAllLayout(code, is_shift)) {
        if (smb == 0) {
            return KEY_TYPE_ANALIZE::NO_SYMBOL_CLEAR;  // something bad
        }
        auto it = std::ranges::find(symbols, smb);
        if (it == symbols.end())
            symbols.push_back(smb);
    }

    if (symbols.empty())
        return KEY_TYPE_ANALIZE::NO_SYMBOL_CLEAR;

    if (symbols.size() == 1) {
        if (std::iswdigit(symbols[0]) || g_settings->treat_as_letter.contains(symbols[0]))
            return KEY_TYPE_ANALIZE::LETTER_OR_SPACE;
        return KEY_TYPE_ANALIZE::SPACE;
    }

    if (g_settings->words_separate_mode == WordsSeparateMode::by_spaces) {
        return KEY_TYPE_ANALIZE::LETTER;
    }

    bool has_letter = false;
    bool has_custom = false;
    for (auto s : symbols) {
        if (std::iswalpha(s))
            has_letter = true;
        else
            has_custom = true;
    }

    if (has_letter) {
        return has_custom ? KEY_TYPE_ANALIZE::LETTER_OR_CUSTOM : KEY_TYPE_ANALIZE::LETTER;
    }

    return KEY_TYPE_ANALIZE::CUSTOM;
}