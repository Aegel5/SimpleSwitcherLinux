#pragma once

enum class KEY_TYPE_ANALIZE{
	CUSTOM,
	LETTER_OR_CUSTOM,
	SPACE,
	LETTER_OR_SPACE,
	LETTER,

    NO_SYMBOL_CLEAR,
    NO_SYMBOL_NO_CLEAR
};



struct RevertKeysData {
    struct Info{
        ScanCode code{};
        bool is_shift = false;
    };
	bool needLanguageChange = false;
	vector<Info> to_inject;
};

