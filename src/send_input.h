#include "keyboard_grab.h"

inline void send_input(ScanCode code, bool is_down) {
    log_debug("send {}/{}. down={}", code, HotKey::CodeToString(code), is_down);
    g_keygrab->AddInject(code, is_down);
}

inline void send_input_press(ScanCode code, int cnt = 1) {
    for (int i = 0; i < cnt; i++) {
        send_input(code, true);
        send_input(code, false);
    }
}

inline void send_input_hk(const HotKey& hk) {
    for(auto code : hk){
        send_input(code, true);
    }
    for(auto code : hk | std::views::reverse){
        send_input(code, false);
    }

}