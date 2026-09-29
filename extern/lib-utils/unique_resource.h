#pragma once

#include <iostream>
#include <utility>

template <typename T, typename Deleter, T invalid_value = T{}>
class unique_res {
    T t;
    [[no_unique_address]] Deleter deleter_{};

   public:
    // Конструктор по умолчанию инициализирует невалидным значением (например, -1)
    unique_res() noexcept : t(invalid_value) {}

    // Конструктор с ресурсом
    explicit unique_res(T _t) noexcept : t(_t) {}

    bool is_valid() const noexcept {return t != invalid_value;}

    void reset(T _t = invalid_value) noexcept {
        if (is_valid()) {
            deleter_(t);
        }
        t = _t;
    }

    explicit operator bool() const { return is_valid(); }

    // Деструктор
    ~unique_res() {
        reset();
    }

    // Запрет копирования
    unique_res(const unique_res&) = delete;
    unique_res& operator=(const unique_res&) = delete;

    // Конструктор перемещения: перемещаем хэндл и кастомные функторы (если они со стейтом)
    unique_res(unique_res&& other) noexcept
        : t(std::exchange(other.t, invalid_value)), deleter_(std::move(other.deleter_)) {}

    // Оператор перемещения
    unique_res& operator=(unique_res&& other) noexcept {
        if (this != &other) {
            reset();
            t = std::exchange(other.t, invalid_value);
            deleter_ = std::move(other.deleter_);
        }
        return *this;
    }

    T& ref_reset() noexcept {
        reset();
        return t;
    }

    // Доступ к ресурсу
    T get() const noexcept { return t; }
};
