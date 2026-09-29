#include "CycleRevertList.h"

std::vector<int> CycleRevertList::GenerateWords(RevertType revertType) {

    // сначала объеденим все одинаковые сохраняя индексы старта слов.

    struct ZippData {
        int i;
        const Info* p;
        KEY_TYPE_ANALIZE type;
        bool is_last_revert = false;
    };

    std::vector<ZippData> zipped;
    zipped.reserve(8);
    {
        Info stumb;
        Info* prev = &stumb;
        for (int i = 0; i < m_symbolList.size(); prev = &m_symbolList[i], i++) {
            const auto& cur = m_symbolList[i];
            auto type = cur.type;

            if (prev->is_last_revert) {
                zipped.emplace_back(i, &cur, type, true);
                continue;
            }

            auto check = [&]() -> bool {
                if (cur.key == prev->key) {  // одинаковый клавиши - всегда обрабатываются одинаково.
                    return false;
                }

                if (is_in(type, KEY_TYPE_ANALIZE::CUSTOM, KEY_TYPE_ANALIZE::LETTER_OR_CUSTOM))
                    return true;

                return type != prev->type;
            };
            if (check()) {
                zipped.emplace_back(i, &cur, type);
            }
        }
    }

    // по сути, все уже готово, осталось лишь решить вопрос possible letter / letter.

    std::vector<int> words;  // индексы старта слов.

    const auto can_separate_posible =
        (revertType == RevertType::last_word && g_settings->words_separate_mode == WordsSeparateMode::smart_analize_always) ||
        (revertType == RevertType::several_words && is_in(g_settings->words_separate_mode, WordsSeparateMode::smart_analize_by_shift,
                            WordsSeparateMode::smart_analize_always));

    for (int i = -1; auto& it : zipped) {
        i++;
        auto check = [&]() -> bool {
            // it.type должен иметь актуальный тип, так как используется на следующих итерациях.
            if (it.type == KEY_TYPE_ANALIZE::LETTER_OR_SPACE) {
                // не разделяем слово без надобности.
                it.type = (i > 0 && i < std::ssize(zipped) - 1 &&
                           is_all(KEY_TYPE_ANALIZE::LETTER, zipped[i - 1].type, zipped[i + 1].type))
                              ? KEY_TYPE_ANALIZE::LETTER
                              : KEY_TYPE_ANALIZE::SPACE;
            }
            if (it.type == KEY_TYPE_ANALIZE::SPACE)
                return false;
            if (it.type == KEY_TYPE_ANALIZE::LETTER_OR_CUSTOM) {
                bool separate = can_separate_posible;
                if (separate) {
                    // выделаем только если слева или справа space
                    separate = i == 0 || i + 1 >= zipped.size() ||
                               is_in(KEY_TYPE_ANALIZE::SPACE, zipped[i - 1].type, zipped[i + 1].type) ||
                               is_in(KEY_TYPE_ANALIZE::CUSTOM, zipped[i - 1].type, zipped[i + 1].type);
                }

                it.type =
                    separate ? KEY_TYPE_ANALIZE::CUSTOM : KEY_TYPE_ANALIZE::LETTER;  // теперь можем определить тип
            }
            if (it.is_last_revert) {  // форсируем разделение.
                return true;
            }
            if (it.type == KEY_TYPE_ANALIZE::LETTER) {
                return i == 0 || zipped[i - 1].type != KEY_TYPE_ANALIZE::LETTER;
            }
            return true;  // custom
        };
        if (check()) {
            words.push_back(it.i);
        }
    }

    return words;
}
