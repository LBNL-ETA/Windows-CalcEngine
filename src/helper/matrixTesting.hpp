#pragma once

#include <vector>

namespace Helper
{
    void compareMatrices(const std::vector<std::vector<double>> & expected,
                         const std::vector<std::vector<double>> & actual,
                         double tolerance = 1e-6);
}