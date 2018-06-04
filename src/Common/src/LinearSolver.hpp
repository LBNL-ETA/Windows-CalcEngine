#ifndef LINEARSOLVER_H
#define LINEARSOLVER_H

#include <vector>

namespace FenestrationCommon
{
    class SquareMatrix;

    class CLinearSolver
    {
    public:
        CLinearSolver();

        std::vector<double> solveSystem(SquareMatrix & t_MatrixA, std::vector<double> & t_VectorB) const;

    private:
        std::vector<double> checkSingularity(SquareMatrix & t_MatrixA) const;
        std::vector<size_t> makeUpperTriangular(SquareMatrix & t_MatrixA) const;
    };
}   // namespace FenestrationCommon

#endif
