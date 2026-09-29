#include <systemd/sd-bus.h>
#include <cstdlib>  // Для std::getenv
#include <cstring>
#include <iostream>
#include <vector>
#include "lib-utils/unique_resource.h"

namespace shell_dbus {

// class UserPrivilegeScope {
//     bool is_switched = false;
// public:
//     UserPrivilegeScope() {
//         if(getuid() != 0) return;
//         seteuid(g_settings->user_id);
//         is_switched = true;
//     }

//     // Деструктор автоматически возвращает права root при выходе из блока {}
//     ~UserPrivilegeScope() {
//         if(!is_switched) return;
//         seteuid(0);
//     }

//     // Запрещаем копирование и присваивание объекта
//     UserPrivilegeScope(const UserPrivilegeScope&) = delete;
//     UserPrivilegeScope& operator=(const UserPrivilegeScope&) = delete;
// };    

using BusPtr = unique_res<sd_bus*, decltype([](auto* p){sd_bus_unref(p);})>;
using MessagePtr = unique_res<sd_bus_message*, decltype([](auto* p){sd_bus_message_unref(p);})>;

// Метод возвращает список коротких кодов раскладок (например, "us", "ru")
std::vector<std::string> get_all_layouts_kde() {
    std::vector<std::string> layouts;
    BusPtr bus;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    MessagePtr reply;

    // // 1. Подключаемся к сессионной шине D-Bus
    int r = sd_bus_open_user(&bus.ref_reset());
    if (r < 0) {
        log_error("sd_bus_open_user fail: {}", r);
        return layouts; // Возвращаем пустой вектор, если D-Bus недоступен
    }

    // 2. Вызываем метод 'getLayoutsList', который возвращает массив структур a(sss)
    r = sd_bus_call_method(bus.get(),
                           "org.kde.keyboard",
                           "/Layouts",
                           "org.kde.KeyboardLayouts",
                           "getLayoutsList",
                           &error,
                           &reply.ref_reset(),
                           "", 
                           nullptr);

    if (r < 0) {
        sd_bus_error_free(&error);
        return layouts; // Возвращаем пустой вектор, если это не KDE
    }

    // 3. Парсим массив структур a(sss)
    // Входим в контейнер массива 'a'
    if (sd_bus_message_enter_container(reply.get(), SD_BUS_TYPE_ARRAY, "(sss)") >= 0) {
        
        // Перебираем все структуры (sss) внутри массива
        while (sd_bus_message_enter_container(reply.get(), SD_BUS_TYPE_STRUCT, "sss") >= 0) {
            const char *code = nullptr;
            const char *variant = nullptr;
            const char *name = nullptr;
            
            // Читаем три строки. Нам нужен только первый параметр (code)
            sd_bus_message_read(reply.get(), "sss", &code, &variant, &name);
            
            if (code) {
                layouts.push_back(code); // Добавляем короткое имя ("us", "ru") в вектор
            }
            
            sd_bus_message_exit_container(reply.get()); // Выходим из текущей структуры
        }
        sd_bus_message_exit_container(reply.get()); // Выходим из массива
    }

    return layouts;
}    

// 1. Функция проверки, что мы находимся в KDE
bool isKdeEnvironment() {
    static bool res = get_all_layouts_kde().size() != 0;
    return res;
}

// 2. Функция попытки переключения через D-Bus KDE
bool tryKdeDbusSwitch() {
    BusPtr bus;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    MessagePtr reply;

    // Подключаемся к сессионной шине
    int r = sd_bus_open_user(&bus.ref_reset());
    if (r < 0)
        return false;

    // Вызываем метод. Если сервиса org.kde.keyboard нет, sd-bus вернет ошибку, но программа НЕ упадет
    r = sd_bus_call_method(bus.get(), "org.kde.keyboard", "/Layouts", "org.kde.KeyboardLayouts", "switchToNextLayout", &error,
                           &reply.ref_reset(), "", nullptr);

    bool success = (r >= 0);

    // Освобождаем ресурсы
    sd_bus_error_free(&error);

    return success;
}

bool try_next_layout() {
    //UserPrivilegeScope _user_pr;
    if (isKdeEnvironment())
        return tryKdeDbusSwitch();
    return false;
}



std::vector<std::string> get_all_layouts(){
    //UserPrivilegeScope _user_pr;
    if(isKdeEnvironment())
        return get_all_layouts_kde();
    return {};
}



}  // namespace shell_dbus