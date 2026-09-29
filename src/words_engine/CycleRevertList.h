#pragma once

#include "common.h"

// Класс текущих набранных символов, разбивает буквы на слова.

class CycleRevertList {

    struct Info {
        ScanCode key = 0;
        bool is_shift = false;
        KEY_TYPE_ANALIZE type{};
        bool is_last_revert = false;
    };

    static const int c_nMaxLettersSave = 90;
    std::deque<Info> m_symbolList;  // просто список всего, что сейчас набрано.
    static const int c_lastCorrectedInf = 9999999;
    int iLastCorrected = c_lastCorrectedInf;

    void ClearGenerated() { iLastCorrected = c_lastCorrectedInf; }

    bool is_all(auto&& first, auto&&... t) { return ((first == t) && ...); }
    std::vector<int> GenerateWords(RevertType);

   public:
    bool HasAnySymbol() const { return !m_symbolList.empty(); }   
    void DeleteLastSymbol() {
        if (!m_symbolList.empty()) {
            bool move_last = m_symbolList.back().is_last_revert;
            m_symbolList.pop_back();
            if (move_last && !m_symbolList.empty())
                m_symbolList.back().is_last_revert = true;
        }
        ClearGenerated();
    }   
    RevertKeysData FillKeyToRevert(RevertType revertType, bool always_full_text = false) {

        RevertKeysData keyList;

        if (m_symbolList.empty()) {
            return keyList;
        }

        auto words = GenerateWords(revertType);

        if (words.empty()) {  // например одни пробелы были введены.
            return keyList;
        }

        int prev_correct = iLastCorrected;

        if (always_full_text) {
            iLastCorrected = words[0];
            keyList.needLanguageChange = true;
        } else if (revertType == RevertType::all) {
            keyList.needLanguageChange = true;
            iLastCorrected = words[0];
            for (int i = ssize(m_symbolList) - 2; i >= 0; --i) {
                if (m_symbolList[i].is_last_revert) {
                    iLastCorrected = i + 1;
                    break;
                }
            }
        } else if (revertType == RevertType::last_word) {
            keyList.needLanguageChange = true;
            iLastCorrected = words[words.size() - 1];
        } else {  // несколько слов
            if (words[0] >= iLastCorrected) {
                iLastCorrected = c_lastCorrectedInf;  // все слова уже изменили, сбрасываем на начало.
            }
            keyList.needLanguageChange = iLastCorrected == c_lastCorrectedInf;
            for (int i = words.size() - 1; i >= 0; i--) {
                if (words[i] < iLastCorrected) {
                    iLastCorrected = words[i];
                    break;
                }
            }
        }

        for (int i = always_full_text ? 0 : iLastCorrected; i < ssize(m_symbolList); ++i) {
            keyList.to_inject.push_back({m_symbolList[i].key, m_symbolList[i].is_shift});
        }

        if (prev_correct == iLastCorrected) {
            iLastCorrected = c_lastCorrectedInf;  // происходит отмена предыдущего реверт, сбрасываемся на начало.
        }

        return keyList;
    }

    // void SetSeparateLast() {
    //     if (!m_symbolList.empty())
    //         m_symbolList.back().is_last_revert = true;
    // }

    void AddKeyToList(ScanCode code, bool is_shift, KEY_TYPE_ANALIZE type) {
        ClearGenerated();

        while (m_symbolList.size() >= c_nMaxLettersSave) {
            m_symbolList.pop_front();
        }

        m_symbolList.push_back({.key = code, .is_shift = is_shift, .type = type});
    }
    int size(){return m_symbolList.size();}
    void Clear() {
        if (!m_symbolList.empty()) {
            m_symbolList.clear();
        }
        ClearGenerated();
    }	
};
