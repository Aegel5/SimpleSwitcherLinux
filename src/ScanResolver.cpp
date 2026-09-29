#include <xkbcommon/xkbcommon.h>
#include "shell_dbus.h"

namespace {
std::unique_ptr<xkb_context, decltype([](auto p) { xkb_context_unref(p); })> ctx{};
struct Store {
    std::unique_ptr<xkb_keymap, decltype([](auto p) { xkb_keymap_unref(p); })> map{};

    Store(string_view layout) {
        struct xkb_rule_names names = {
            .rules = nullptr, .model = nullptr, .layout = layout.data(), .variant = nullptr, .options = nullptr};
        map.reset(xkb_keymap_new_from_names(ctx.get(), &names, XKB_KEYMAP_COMPILE_NO_FLAGS));
    }
};

vector<Store> stores;
bool inited = false;
}  // namespace

std::generator<wchar_t> ResolveForAllLayout(ScanCode code, bool is_shift) {
    
    if (!inited) {
        inited = true;        
        auto all_layouts = shell_dbus::get_all_layouts();
        if(all_layouts.empty()){
            all_layouts = g_settings->fallback_layouts_list;
        }else{
            log_debug("found shell layouts");
        }
        if(all_layouts.empty()){
            log_error("layouts not found");
        }
        ctx.reset(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
        if (ctx) {
            for (auto it : all_layouts) {
                log_debug("init layout {}", it);
                stores.emplace_back(it);
            }
        }

    }

    code += 8;
    xkb_level_index_t level = is_shift ? 1 : 0;
    for (const auto& it : stores) {
        const xkb_keysym_t* syms;
        if (!it.map)
            co_yield 0;
        int num_syms = xkb_keymap_key_get_syms_by_level(it.map.get(), code, 0, level, &syms);
        if (num_syms <= 0 || !syms) {
            co_yield 0;
        }
        auto res = xkb_keysym_to_utf32(*syms);
        co_yield res;
    }
}