#include "linux/input-event-codes.h"

using std::pair;
namespace {

std::map<string_view, pair<ScanCode, bool>, decltype([](string_view lhs, string_view rhs) {
             return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](auto a, auto b) {
                 return StrUtils::ToLowerEnglishQuick(a) < StrUtils::ToLowerEnglishQuick(b);
             });
         })>
    m1;

struct Key{
    ScanCode c;
    bool common;
    auto operator<=>(const Key& k) const = default;
};

std::map<Key, string_view> m2;
//std::map<ScanCode, string_view> m_common;



void bind(string_view s, ScanCode c, bool common = false) {
    m1[s] = {c, common};
    m2[Key{c, common}] = s;
}

void init() {
    bind("Shift", KEY_LEFTSHIFT, true);
    bind("LShift", KEY_LEFTSHIFT);
    bind("RShift", KEY_RIGHTSHIFT);

    bind("Alt", KEY_LEFTALT, true);
    bind("LAlt", KEY_LEFTALT);
    bind("RAlt", KEY_RIGHTSHIFT);

    bind("Ctrl", KEY_LEFTCTRL, true);
    bind("LCtrl", KEY_LEFTCTRL);
    bind("RCtrl", KEY_RIGHTCTRL);

    bind("Meta", KEY_LEFTMETA, true);
    bind("LMeta", KEY_LEFTMETA);
    bind("RMeta", KEY_RIGHTMETA);

    // todo - use reflaction c++26 for other

    bind("Capslock", KEY_CAPSLOCK);
    bind("Backspace", KEY_BACKSPACE);
    bind("Break", KEY_BREAK);
    bind("Esc", KEY_ESC);
    bind("PageUp", KEY_PAGEUP);
    bind("PageDown", KEY_PAGEDOWN);
    bind("Space", KEY_SPACE);

    bind("F1", KEY_F1);
    bind("F2", KEY_F2);
    bind("F3", KEY_F3);
    bind("F4", KEY_F4);
    bind("F5", KEY_F5);
    bind("F6", KEY_F6);
    bind("F7", KEY_F7);
    bind("F8", KEY_F8);
    bind("F9", KEY_F9);
    bind("F10", KEY_F10);
    bind("F11", KEY_F11);
    bind("F12", KEY_F12);
    bind("F13", KEY_F13);
    bind("F14", KEY_F14);
    bind("F15", KEY_F15);
    bind("F16", KEY_F16);
    bind("F17", KEY_F17);
    bind("F18", KEY_F18);
    bind("F19", KEY_F19);
    bind("F20", KEY_F20);
    bind("F21", KEY_F21);
    bind("F22", KEY_F22);
    bind("F23", KEY_F23);
    bind("F24", KEY_F24);

    bind("A", KEY_A);
    bind("B", KEY_B);
    bind("C", KEY_C);
    bind("D", KEY_D);
    bind("E", KEY_E);
    bind("F", KEY_F);
    bind("G", KEY_G);
    bind("H", KEY_H);
    bind("I", KEY_I);
    bind("J", KEY_J);
    bind("K", KEY_K);
    bind("L", KEY_L);
    bind("M", KEY_M);
    bind("N", KEY_N);
    bind("O", KEY_O);
    bind("P", KEY_P);
    bind("Q", KEY_Q);
    bind("R", KEY_R);
    bind("S", KEY_S);
    bind("T", KEY_T);
    bind("U", KEY_U);
    bind("V", KEY_V);
    bind("W", KEY_W);
    bind("X", KEY_X);
    bind("Y", KEY_Y);
    bind("Z", KEY_Z);

    bind("0", KEY_0);
    bind("1", KEY_1);
    bind("2", KEY_2);
    bind("3", KEY_3);
    bind("4", KEY_4);
    bind("5", KEY_5);
    bind("6", KEY_6);
    bind("7", KEY_7);
    bind("8", KEY_8);
    bind("9", KEY_9);
}

bool dummy = (init(), true);

}  // namespace
string_view to_string(ScanCode code, bool common) {
    auto it = m2.find(Key{code, common});
    if (it != m2.end()) return it->second;

    if(common){
        it = m2.find(Key{code, false});
    }

    if(it != m2.end()) return it->second;

    return {};
}
pair<ScanCode, bool> from_string(string_view s) {
    auto it = m1.find(s);
    if (it == m1.end()) {
        return {};
    }
    return it->second;
}