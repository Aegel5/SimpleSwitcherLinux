
#include "CycleRevertList.h"
#include "common.h"
#include "send_input.h"
#include "shell_dbus.h"

extern KEY_TYPE_ANALIZE AnalizeCode(ScanCode code, bool is_shift);

namespace words_engine {

CycleRevertList list;
HotKey state;

void clear_words() {
    // log_any("clear words");
    list.Clear();
}

void DoRevert(RevertType mode) {
    log_always("do_revert {} symbols", list.size());

    if (!list.HasAnySymbol())
        return;

    auto res = list.FillKeyToRevert(mode);
    if (res.needLanguageChange) {
        log_debug("change layout");
        if (shell_dbus::try_next_layout()) {
            log_debug("use dbus");
        } else {
            log_debug("fallback emulate");
            send_input_hk(g_settings->fallback_emulate_switch);
            g_keygrab->AddPause();
        }
    }
    log_debug("inject backspaces {}", res.to_inject.size());
    send_input_press(KEY_BACKSPACE, res.to_inject.size());

    log_debug("inject keys");
    for (const auto& it : res.to_inject) {
        if (it.is_shift) {
            send_input(KEY_LEFTSHIFT, true);
        }
        send_input_press(it.code);
        if (it.is_shift) {
            send_input(KEY_LEFTSHIFT, false);
        }
    }
}

void ProcessType(ScanCode code, bool is_down) {
    code = HotKey::to_left(code);

    if (is_down) {
        if (!state.empty() && !HotKey::IsMod(state.Key()))
            state.pop_back();  // fix for quick typing
        state.Add(code);
    } else {
        state.Remove(code);
    }

    if (!is_down)
        return;

    if (!std::ranges::any_of(state, [](auto key) { return !HotKey::IsMod(key); }))
        return;

    if (state.size() >= 3) {
        list.Clear();
        return;
    }

    bool is_shift = false;

    if (state.size() == 2) {
        if (*state.begin() == KEY_LEFTSHIFT) {
            is_shift = true;
        } else {
            list.Clear();
            return;
        }
    }

    if (code == KEY_BACKSPACE) {
        list.DeleteLastSymbol();
        return;
    }

    auto type = AnalizeCode(code, is_shift);
    if (type == KEY_TYPE_ANALIZE::NO_SYMBOL_CLEAR) {
        list.Clear();
        return;
    }
    if (type == KEY_TYPE_ANALIZE::NO_SYMBOL_NO_CLEAR)
        return;

    list.AddKeyToList(code, is_shift, type);
}
}  // namespace words_engine