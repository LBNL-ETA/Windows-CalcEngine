#pragma once

#include <vector>
#include "gtest/gtest.h"

namespace Helper
{
    void compareMatrices(const std::vector<std::vector<double>> & expected,
                         const std::vector<std::vector<double>> & actual,
                         double tolerance = 1e-6);
}