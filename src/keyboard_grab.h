#include <linux/input.h>
#include "file_utils.h"

class keyboard_grab{
    unique_fd fd_src;
    unique_fd fd_dst;
    bool grabbed = false;
    bool created = false;    
    vector<input_event> to_inject{};    
    vector<input_event> events;
    DISALLOW_COPY_MOVE_AND_ASSIGN(keyboard_grab);
public:    
    keyboard_grab() = default;
    int get_fd() {return fd_src.get();}
    bool init();
    bool process_all();
    void clear();
    ~keyboard_grab();
    void AddInject(ScanCode code, bool is_down);
    void AddPause(){
        to_inject.push_back(input_event{.type = 999});
    }
};

inline keyboard_grab* g_keygrab = nullptr;