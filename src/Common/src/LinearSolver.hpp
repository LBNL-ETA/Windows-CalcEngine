#pragma once

#include <vector>

namespace FenestrationCommon
{
    class SquareMatrix;

    std::vector<double> solveSystem(const SquareMatrix& t_MatrixA, const std::vector<double> & t_VectorB);
}   // namespace FenestrationCommon
