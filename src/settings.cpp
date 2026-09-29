#include <unistd.h>
#include <filesystem>
#include <iostream>

#include "glaze/json.hpp"
#include "settings.h"

namespace fs = std::filesystem;

static std::string linux_wstring_to_string(const std::wstring& wstr) {
    if (wstr.empty())
        return {};
    size_t size_needed = std::wcstombs(nullptr, wstr.c_str(), 0);
    if (size_needed == static_cast<size_t>(-1)) {
        return {};  // Ошибка кодирования
    }
    std::string result(size_needed, '\0');
    std::wcstombs(result.data(), wstr.c_str(), size_needed + 1);
    return result;
}

static std::wstring linux_string_to_wstring(const std::string_view str) {
    if (str.empty())
        return {};
    std::mbstate_t state{};
    const char* src = str.data();
    size_t src_len = str.size();
    size_t size_needed = mbsnrtowcs(nullptr, &src, src_len, 0, &state);
    if (size_needed == static_cast<size_t>(-1)) {
        return {};  // Ошибка кодирования
    }
    std::wstring result(size_needed, L'\0');
    state = std::mbstate_t{};
    src = str.data();
    mbsnrtowcs(result.data(), &src, src_len, size_needed, &state);
    return result;
}

// =================================================================
// 2. ИНТЕГРАЦИЯ В ШАБЛОНЫ GLAZE
// =================================================================

template <>
struct glz::to<glz::JSON, std::wstring> {
    template <auto Opts>
    static void op(const std::wstring& wstr, is_context auto&& ctx, auto&&... args) noexcept {
        glz::to<glz::JSON, std::string>::template op<Opts>(linux_wstring_to_string(wstr), ctx, args...);
    }
};

template <>
struct glz::from<glz::JSON, std::wstring> {
    template <auto Opts>
    static void op(std::wstring& wstr, is_context auto&& ctx, auto&&... args) noexcept {
        string_view utf8_str;
        glz::from<glz::JSON, string_view>::template op<Opts>(utf8_str, ctx, args...);
        wstr = linux_string_to_wstring(utf8_str);
    }
};

static fs::path get_exe_parent_dir() {
    // Получаем абсолютный путь к самому exe-файлу через /proc/self/exe
    fs::path exe_path = fs::read_symlink("/proc/self/exe");
    // Переходим на 1 уровень вверх (сначала к папке с exe, потом к её родителю)
    return exe_path.parent_path().parent_path();
}

template <>
struct glz::from<glz::JSON, HotKey> {
    template <auto Opts>
    static void op(HotKey& wstr, is_context auto&& ctx, auto&&... args) noexcept {
        string_view utf8_str;
        glz::from<glz::JSON, string_view>::template op<Opts>(utf8_str, ctx, args...);
        wstr.FromString(utf8_str);
    }
};

template <>
struct glz::to<glz::JSON, HotKey> {
    template <auto Opts>
    static void op(const HotKey& obj, is_context auto&& ctx, auto&&... args) noexcept {
        glz::to<glz::JSON, std::string>::template op<Opts>(obj.ToString(), ctx, args...);
    }
};

namespace settings {
bool save() {
    auto path = get_exe_parent_dir() / "SimpleSwitcher.json";
    static constexpr glz::opts write_options{
        .prettify = true,  // Включает форматирование (человекочитаемый вид)
    };
    auto ec = glz::write_file_json<write_options>(g_settings, path.c_str(), string{});
    if (ec) {
        log_error("error create json {}", ec.custom_error_message);
        return false;
    }
    return true;
}

bool load() {
    auto path = get_exe_parent_dir() / "SimpleSwitcher.json";
    if (!fs::exists(path)) {
        log_always("settings.json not exists. create it");
        save();
        return true;
    }

    auto ec = glz::read_file_json(g_settings, path.c_str(), string{});
    if (ec) {
        log_error("error read json {}", ec.custom_error_message);
        return false;
    }
    return true;
}

}  // namespace settings