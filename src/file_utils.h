#pragma once

#include <unistd.h>
#include "lib-utils/unique_resource.h"

inline constexpr auto __close_fd_lambda = [](auto x) { close(x); };
using unique_fd = unique_res<int, decltype(__close_fd_lambda), -1>;