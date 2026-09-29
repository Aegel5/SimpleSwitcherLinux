#include <signal.h>
#include "keyboard_grab.h"
#include "mouse_hook.h"

extern void main_loop();

// Хэндлер для безопасного восстановления при падениях или Ctrl+C
void crash_cleanup(int sig) {
    
    if (g_keygrab)
        g_keygrab->clear();  // return input to keyboard on crash

    // Разделяем штатное завершение и краш
    if (sig == SIGTERM || sig == SIGINT) {
        // Мы успешно выполнили очистку по просьбе системы
        _exit(0); 
    } else {
        // Это был краш (например, SIGSEGV), возвращаем код ошибки для bash/systemd
        _exit(128 + sig); 
    }
}

int main(int argc, char* argv[]) {
    log_always("SimpleSwitcher start");
    setlocale(LC_ALL, "en_US.utf8");
    g_settings.reset(new Settings());

    if(!settings::load()){
        return 1;
    }

    {
        // Перебираем все аргументы (argv[0] — это имя самой программы)
        for (int i = 1; i < argc; ++i) {
            if (std::string_view(argv[i]) == "-cfg") {
                settings::save();
                log_always("config updated");
                return 0;
            }
        }
    }    

    keyboard_grab key_grabber;
    g_keygrab = &key_grabber;
    mouse_grabber mouse_hook;
    g_mouse_hook = &mouse_hook;

    {
        // Используем современный sigaction вместо устаревшего signal
        struct sigaction sa;
        sa.sa_handler = crash_cleanup;
        sigemptyset(&sa.sa_mask);

        // SA_RESTART позволяет системным вызовам продолжаться после сигналов остановки
        sa.sa_flags = SA_RESTART;

        int signals[] = {SIGINT, SIGTERM, SIGQUIT, SIGSEGV, SIGILL, SIGFPE, SIGBUS};

        // Безопасный подсчет размера массива в C++
        for (int sig : signals) {
            sigaction(sig, &sa, nullptr);
        }
    }

    main_loop();

    log_always("SimpleSwitcher exit");
    return 0;
}