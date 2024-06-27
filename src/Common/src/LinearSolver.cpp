#include <stdexcept>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <cmath>

#include "LinearSolver.hpp"
#include "SquareMatrix.hpp"

namespace FenestrationCommon
{
    std::vector<double> solveSystem(const SquareMatrix & t_MatrixA,
                                    const std::vector<double> & t_VectorB)
    {
        if(t_MatrixA.size() != t_VectorB.size())
        {
            throw std::runtime_error(
              "Matrix and vector for system of linear equations are not the same size.");
        }

        // Make a copy of t_VectorB to work with
        std::vector<double> resultVector = t_VectorB;

        // Make a copy of t_MatrixA to work with (if necessary, depending on makeUpperTriangular
        // implementation)
        SquareMatrix matrixA = t_MatrixA;

        std::vector<size_t> index = matrixA.makeUpperTriangular();
        const size_t size = matrixA.size();

        int ii = -1;
        for(size_t i = 0; i < size; ++i)
        {
            size_t ll = index[i];
            std::swap(resultVector[ll], resultVector[i]);
            double sum = resultVector[i];
            if(ii != -1)
            {
                for(size_t j = ii; j < i; ++j)
                {
                    sum -= matrixA(i, j) * resultVector[j];
                }
            }
            else if(sum != 0.0)
            {
                ii = static_cast<int>(i);
            }
            resultVector[i] = sum;
        }

        for(int i = static_cast<int>(size) - 1; i >= 0; --i)
        {
            double sum = resultVector[i];
            for(size_t j = i + 1; j < size; ++j)
            {
                sum -= matrixA(i, j) * resultVector[j];
            }
            resultVector[i] = sum / matrixA(i, i);
        }

        return resultVector;
    }

}   // namespace FenestrationCommon
