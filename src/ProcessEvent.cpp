#include <linux/input.h>
#include "send_input.h"
#include "mouse_hook.h"
#include "words_engine/words_engine.h"

namespace {
HotKey hk_current;
HotKey prev;
uint64_t mouse_counter {};
}

void clear_state(){
    for(auto it: prev){
        send_input(it, false);
    }
}

// true - запретить
bool ProcessEvent(ScanCode code, bool is_down) {

    if(mouse_counter != g_mouse_hook->GetLastId()){
        mouse_counter = g_mouse_hook->GetLastId();
        words_engine::clear_words();
    }

    if (is_down) {
        prev = hk_current;
        hk_current.Add(code);        
        if (hk_current.Compare(g_settings->hk_last_word)) {
            clear_state();
            words_engine::DoRevert(RevertType::last_word);
            return true;
        }
        else if (hk_current.Compare(g_settings->hk_several_words)) {
            clear_state();
            words_engine::DoRevert(RevertType::several_words);
            return true;
        }
        else if (hk_current.Compare(g_settings->hk_all_text)) {
            clear_state();
            words_engine::DoRevert(RevertType::all);
            return true;
        }        
    }else{
        hk_current.Remove(code);
    }

    words_engine::ProcessType(code, is_down);

    return false;
}