#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>


#include "keyboard_grab.h"

extern bool ProcessEvent(ScanCode code, bool is_up);

static string FindKeyboardDevices() {
    namespace fs = std::filesystem;
    std::string path = "/dev/input/by-id";

    if (!fs::exists(path)) {
        return {};
    }

    std::vector<std::string> keyboards;
    for (const auto& entry : fs::directory_iterator(path)) {
        std::string filename = entry.path().filename().string();

        // Ищем файлы, имя которых заканчивается на "-event-kbd"
        if (filename.length() >= 10 && filename.compare(filename.length() - 10, 10, "-event-kbd") == 0) {
            keyboards.push_back(entry.path().string());
            log_debug("found keyboard: {}", keyboards.back());
        }
    }

    return keyboards.empty() ? "__error_not_found" : keyboards[0];
}

bool keyboard_grab::init() {
    // 1. Открываем физическую клавиатуру по умолчанию
    auto keybr = !g_settings->keyboard_device.empty() ? g_settings->keyboard_device : FindKeyboardDevices();
    log_always("try hook keyboard: {}", keybr);
    fd_src.reset(open(keybr.c_str(), O_RDONLY | O_NONBLOCK));
    if (!fd_src) {
        log_error("Не удалось открыть клавиатуру по умолчанию");
        return false;
    }

    if (g_settings->keyboard_exclusive_mode) {
        // 2. Эксклюзивный захват
        if (ioctl(fd_src.get(), EVIOCGRAB, 1) < 0) {
            log_error("Ошибка захвата клавиатуры (нужен sudo?)");
            return false;
        }
        grabbed = true;
    }

    // 3. Открываем uinput
    fd_dst.reset(open("/dev/uinput", O_WRONLY | O_NONBLOCK));
    if (!fd_dst) {
        log_error("can't open /dev/uinput");
        return false;
    }

    // 4. Инициализация виртуальной клавиатуры
    ioctl(fd_dst.get(), UI_SET_EVBIT, EV_KEY);
    ioctl(fd_dst.get(), UI_SET_EVBIT, EV_SYN);
    for (int i = 0; i < KEY_MAX; i++) {
        ioctl(fd_dst.get(), UI_SET_KEYBIT, i);
    }

    struct uinput_setup usetup{};
    usetup.id.bustype = BUS_USB;
    strcpy(usetup.name, "Pure Proxy Keyboard");

    if (ioctl(fd_dst.get(), UI_DEV_SETUP, &usetup) < 0 || ioctl(fd_dst.get(), UI_DEV_CREATE) < 0) {
        log_error("can't create keybr device");
        return false;
    }

    created = true;

    return true;
}

void keyboard_grab::clear() {
    if (fd_src) {
        if (grabbed) {
            ioctl(fd_src.get(), EVIOCGRAB, 0);
            grabbed = false;
        }
        fd_src.reset();
    }

    if (fd_dst) {
        if (created){
            ioctl(fd_dst.get(), UI_DEV_DESTROY);
            created = false;
        }
        fd_dst.reset();
    }
}

keyboard_grab::~keyboard_grab() {
    clear();
}

bool keyboard_grab::process_all() {
    while (true) {
        if (events.size() >= 1000) {
            log_error("events.size() >= 1000");
            return false;
        }

        {
            input_event ev_read;
            if (read(fd_src.get(), &ev_read, sizeof(struct input_event)) < 0) {
                // EAGAIN или EWOULDBLOCK означают, что мы выгребли всё дочиста
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return true;
                }
                // Если произошла реальная ошибка чтения (например, мышь отключили)
                log_error("keyboard error read");
                return false;
            }
            log_debug("code {}, value {}, type {}", ev_read.code, ev_read.value, ev_read.type);
            if (!is_in(ev_read.type, EV_SYN, EV_KEY)) {
                continue;  // ignore all except key + syn
            }
            ev_read.time = {};  // auto time
            events.push_back(ev_read);
        }

        if (events.back().type == EV_SYN) {
            // Process transaction

            to_inject.clear();

            bool has_data = false;
            for (auto& ev : events) {
                bool skip = false;

                if (ev.type == EV_SYN) {
                    skip = !has_data;
                } else {
                    // Обрабатываем только реальное нажатие (1) и отпускание (0), игнорируем автоповтор (2)
                    skip =
                        ev.type == EV_KEY && (ev.value == 1 || ev.value == 0) && ProcessEvent(ev.code, ev.value == 1);
                }

                if (!skip) {
                    has_data = true;
                    if (g_settings->keyboard_exclusive_mode) {
                        if (write(fd_dst.get(), &ev, sizeof(input_event)) < 0) {
                            log_error("can't write to fd_dst");
                            return false;
                        }
                    }
                }
            }

            for (auto& ev : to_inject) {
                if (ev.type == 999) {
                    log_debug("pause");
                    std::this_thread::sleep_for(25ms);
                    continue;
                } else {
                    // std::this_thread::sleep_for(1ms);
                }

                if (write(fd_dst.get(), &ev, sizeof(input_event)) < 0) {
                    return false;
                }
                ev.code = SYN_REPORT;
                ev.type = EV_SYN;
                ev.value = 0;
                ev.time = {};
                if (write(fd_dst.get(), &ev, sizeof(input_event)) < 0) {
                    return false;
                }
            }

            // transaction done
            events.clear();
            to_inject.clear();
        }
    }
}

void keyboard_grab::AddInject(ScanCode code, bool is_down) {
    auto& ev = to_inject.emplace_back();
    // ev.time = last_time;
    ev.code = code;
    ev.type = EV_KEY;
    ev.value = is_down ? 1 : 0;
}
