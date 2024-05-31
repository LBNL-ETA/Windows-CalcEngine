#pragma once

#include <vector>

namespace FenestrationCommon
{
    class SquareMatrix;

    std::vector<double> solveSystem(SquareMatrix t_MatrixA, std::vector<double> & t_VectorB);
}   // namespace FenestrationCommon
