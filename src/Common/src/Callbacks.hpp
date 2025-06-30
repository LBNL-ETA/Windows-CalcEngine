#pragma once

#include <functional>

namespace FenestrationCommon
{
    using ProgressCallback = std::function<void(size_t current, size_t total)>;
}
