#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include "file_utils.h"


class mouse_grabber {
private:
    DISALLOW_COPY_MOVE_AND_ASSIGN(mouse_grabber);
    unique_fd fd;
    unsigned long long click_count = 0;

public:
    int get_fd() {return fd.get();}
    mouse_grabber() = default;
    // Конструктор открывает устройство в НЕБЛОКИРУЮЩЕМ режиме
    bool init();

    // Выгребает абсолютно все накопившиеся события из буфера ядра
    bool ProcessAll();

    // Возвращает текущее значение счетчика кликов
    unsigned long long GetLastId() const {
        return click_count;
    }

};

inline mouse_grabber* g_mouse_hook = nullptr;
