#include "hot_key.h"

extern string_view to_string(ScanCode code, bool common);
extern pair<ScanCode, bool> from_string(string_view s);

string HotKey::CodeToString(ScanCode code, bool common) {
    return string{to_string(code, common)};
}

void HotKey::FromString(string_view s) {
    clear();
    if (s.empty())
        return;
    auto sElems = StrUtils::Split(s, '+');
    for (auto& sCur : sElems) {
        StrUtils::ToLowerEnglishQuick(sCur);
        if (StrUtils::replaceAll(sCur, "#up", "")) {
            SetUp(true);
        }
        StrUtils::Trim(sCur);
        auto [code, common] = from_string(sCur);
        if (code == 0) {
            clear();
            return;
        }
        Add(code);
        if (common)
            Set_is_common(true);
    }
}
string HotKey::ToString() const {
		std::string s;
		if (size() == 0)
			return s;

		for (int i = 0; i < size(); ++i) {
			auto cur = to_string(codes[i], is_common());
            if(cur.empty()) return {};
            s += cur;
			if (i != size()-1)
				s += " + ";
		}

		// if (m_keyup) {
		// 	s += " #up";
		// }
		// if (m_double_press) {
		// 	s += " #double";
		// }
		return s;
}

