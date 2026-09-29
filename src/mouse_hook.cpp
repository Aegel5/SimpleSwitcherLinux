#include "mouse_hook.h"

static std::string FindMouseDevice() {
    namespace fs = std::filesystem;

    auto find = []() -> string {
        std::string path = "/dev/input/by-id";

        if (!fs::exists(path)) {
            return "";
        }

        for (const auto& entry : fs::directory_iterator(path)) {
            std::string filename = entry.path().filename().string();

            // Ищем файлы, имя которых заканчивается на "-event-mouse"
            if (filename.length() >= 12 && filename.compare(filename.length() - 12, 12, "-event-mouse") == 0) {
                // Возвращаем абсолютный путь к устройству
                return entry.path().string();
            }
        }

        return "";  // Мышь не найдена
    };

    auto res = find();
    log_debug("found mouse: {}", res);
    return res;
}


bool mouse_grabber::init() {
    auto dev_path = !g_settings->mouse_device.empty() ? g_settings->mouse_device : FindMouseDevice();
    log_always("try hook mouse: {} ", dev_path);
    // O_NONBLOCK обязателен для работы цикла выгребания данных
    fd.reset(open(dev_path.c_str(), O_RDONLY | O_NONBLOCK));
    if (!fd) {
        log_error("Ошибка открытия устройства мыши (запустите от sudo)");
        return false;
    }
    return true;
}
bool mouse_grabber::ProcessAll() {
    if (!fd)
        return false;

    // Буфер для чтения событий пачками (для экономии системных вызовов)
    struct input_event evs[64];

    // Цикл работает до тех пор, пока в буфере ядра есть данные
    while (true) {
        ssize_t bytes_read = read(fd.get(), evs, sizeof(evs));

        if (bytes_read < 0) {
            // EAGAIN или EWOULDBLOCK означают, что мы выгребли всё дочиста
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return true;
            }
            // Если произошла реальная ошибка чтения (например, мышь отключили)
            log_error("can't read mouse");
            return false;
        }

        // Если прочитано 0 байт — достигнут конец файла (устройство закрылось)
        if (bytes_read == 0) {
            log_error("device closed");
            return false;
        }

        // Вычисляем, сколько структур input_event мы прочитали за этот шаг
        size_t num_events = bytes_read / sizeof(struct input_event);

        for (size_t i = 0; i < num_events; ++i) {
            // Фильтр: тип EV_KEY (кнопка) и значение 1 (только нажатие)
            if (evs[i].type == EV_KEY && evs[i].value == 1) {
                click_count++;
            }
        }
    }
}
