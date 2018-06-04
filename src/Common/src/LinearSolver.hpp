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
        
    };
}   // namespace FenestrationCommon

#endif
