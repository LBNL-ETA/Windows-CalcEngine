#ifndef TARCOGQBALANCE_H
#define TARCOGQBALANCE_H

#include <memory>
#include <vector>
#include "IGU.hpp"

namespace FenestrationCommon
{
    class SquareMatrix;
    class CLinearSolver;

}   // namespace FenestrationCommon

namespace Tarcog
{
    class CBaseLayer;

    class CHeatFlowBalance
    {
    public:
        explicit CHeatFlowBalance(CIGU & t_IGU);

        std::vector<double> calcBalanceMatrix();

    private:
        void buildCell(std::shared_ptr<CBaseLayer> const & t_Previous,
                       std::shared_ptr<CBaseLayer> const & t_Current,
                       std::shared_ptr<CBaseLayer> const & t_Next,
                       int t_Index);

        std::shared_ptr<FenestrationCommon::SquareMatrix> m_MatrixA;
        std::shared_ptr<std::vector<double>> m_VectorB;
        std::shared_ptr<FenestrationCommon::CLinearSolver> m_LinearSolver;

        CIGU & m_IGU;
    };

}   // namespace Tarcog

#endif
