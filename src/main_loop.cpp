#include <mqueue.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include "keyboard_grab.h"
#include "mouse_hook.h"
#include "words_engine/words_engine.h"

extern bool ProcessEvent(ScanCode code, bool is_up);

static void test() {
    auto type = [](ScanCode c) {
        ProcessEvent(c, true);
        ProcessEvent(c, false);
    };

    type(KEY_A);
    type(KEY_SPACE);
    type(KEY_B);

    type(KEY_CAPSLOCK);

    ProcessEvent(KEY_LEFTSHIFT, true);
    ProcessEvent(KEY_CAPSLOCK, true);
    ProcessEvent(KEY_CAPSLOCK, false);
    ProcessEvent(KEY_LEFTSHIFT, false);
}

void main_loop() {
    if (getuid() == 0) {
        log_always("running from root!");
        setenv("DBUS_SESSION_BUS_ADDRESS", std::format("unix:path=/run/user/{}/bus", g_settings->user_id).c_str(), 1);
    }

#ifndef NDEBUG
    if (getuid() != 0) {
        test();
        return;
    }
#endif

    log_debug("sleep...");
    std::this_thread::sleep_for(1000ms);
    log_debug("start cycle. user {}", geteuid());

    if (!g_keygrab->init()) {
        return;
    }
    if (!g_mouse_hook->init()) {
        return;
    }

    if (setuid(g_settings->user_id) != 0) {
        log_error("can't erase root privl. setuid error {}", errno);
        return;
    }
    // seteuid(g_settings->user_id); // no more needed

    mq_attr attr{.mq_maxmsg = 10, .mq_msgsize = 1};
    mqd_t mq = mq_open("/SimpleSwitcher_queue", O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
    if (mq < 0) {
        log_error("can't open queue {}", std::strerror(errno));
        return;
    }

    struct pollfd fds[3] = {};

    // Настраиваем первый элемент на клавиатуру
    fds[0].fd = g_keygrab->get_fd();
    fds[0].events = POLLIN;

    // Настраиваем второй элемент на мышь
    fds[1].fd = g_mouse_hook->get_fd();
    fds[1].events = POLLIN;

    fds[2].fd = mq;
    fds[2].events = POLLIN;

    while (true) {
        int ret = poll(fds, std::size(fds), 1000);
        if (ret < 0) {
            if (errno == EINTR) {
                log_debug("Вызов прерван сигналом, перезапуск...");
                continue;
            } else {
                log_error("Критическая ошибка poll: {}", std::strerror(errno));
                return;
            }
        }
        if (ret > 0) {
            if (fds[2].revents & POLLIN) {
                while (true) {
                    char ch;
                    // 2. Проверяем чтение из очереди
                    if (mq_receive(mq, &ch, 1, nullptr) <= 0) {
                        if (errno == EAGAIN) {
                            break;  // Символы закончились, выходим из цикла обратно в poll()
                        }
                        std::cerr << "Реальная ошибка mq_receive: " << std::strerror(errno) << std::endl;
                        break;
                    }
                    std::cout << "Получен символ: " << ch << std::endl;
                    if (ch == 'c') {
                        words_engine::clear_words();
                    }
                }
            }

            // Проверяем мышь
            if (fds[1].revents & POLLIN) {
                if (!g_mouse_hook->ProcessAll())
                    return;
            }

            // Проверяем клавиатуру
            if (fds[0].revents & POLLIN) {
                if (!g_keygrab->process_all())
                    return;
            }
        }
    }
}