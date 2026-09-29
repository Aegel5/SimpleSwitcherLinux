#pragma once 

enum class WordsSeparateMode {
	by_spaces = 0,
	separate_symbols = 1,
	smart_analize_by_shift = 2,
	smart_analize_always = 3,
};

struct Settings{

    // devices to grab
    string keyboard_device; // empty for autosearch
    string mouse_device; // empty for autosearch
    bool keyboard_exclusive_mode = false;
    int user_id {1000};

    std::vector<string> fallback_layouts_list {"us", "ru"}; 
    HotKey fallback_emulate_switch {"Alt + Shift"};    

    // switch settings
    HotKey hk_last_word {"Capslock"};
    HotKey hk_several_words {"Shift + Capslock"};
    HotKey hk_all_text {"Ctrl + Capslock"};    
    WordsSeparateMode words_separate_mode {WordsSeparateMode::separate_symbols};
    std::wstring treat_as_letter = L"-_";    

};

inline std::unique_ptr<Settings> g_settings {};

namespace settings {
    bool load();
    bool save();
}