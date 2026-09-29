// std
#include <algorithm>
#include <chrono>
#include <deque>
#include <filesystem>
#include <format>
#include <generator>
#include <map>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

using namespace std::chrono_literals;

// linux
#include "linux/input-event-codes.h"





// our
#define FORWARD(x) static_cast<decltype(x)&&>(x)

#define MAKE_MOVE_DEFAULT(TypeName) \
    TypeName(TypeName&&) = default; \
    TypeName& operator=(TypeName&&) = default

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete;    \
    TypeName& operator=(const TypeName&) = delete;

#define DISALLOW_COPY_MOVE_AND_ASSIGN(TypeName)    \
    TypeName(const TypeName&) = delete;            \
    TypeName& operator=(const TypeName&) = delete; \
    TypeName(TypeName&&) = delete;                 \
    TypeName& operator=(TypeName&&) = delete

// utils
using std::pair;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::vector;
#include "str_utils.h"
bool is_in(auto&& first, auto&&... t) {
    return ((first == t) || ...);
}

// common types

enum class RevertType { last_word, several_words, all };

template<typename... Args>
inline void log_always(const std::format_string<Args...> s, Args&&... v) { 
    std::println(s, std::forward<Args>(v)...); 
}

template<typename... Args>
inline void log_debug(const std::format_string<Args...> s, Args&&... v) { 
#ifndef NDEBUG
    log_always(s, FORWARD(v)...);
#endif
}

template<typename... Args>
inline void log_error(const std::format_string<Args...> s, Args&&... v) { 
    std::println(stderr, s, std::forward<Args>(v)...);
}

using ScanCode = uint32_t;

#include "hot_key/hot_key.h"

#include "settings.h"
