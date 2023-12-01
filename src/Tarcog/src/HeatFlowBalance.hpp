#pragma once

#include <memory>
#include <vector>

#include <WCECommon.hpp>

#include "IGU.hpp"

namespace FenestrationCommon
{
    class SquareMatrix;
    class CLinearSolver;

}   // namespace FenestrationCommon


namespace Tarcog::ISO15099
{
    class CBaseLayer;
    class CEnvironment;

    class CHeatFlowBalance
    {
    public:
        explicit CHeatFlowBalance(CIGU & t_IGU);

        std::vector<double> calcBalanceMatrix();

    private:
        void buildBaseCell(size_t sP,
                           double hgl,
                           double hgap_prev,
                           double hgap_next,
                           double qv_prev,
                           double qv_next,
                           double solarRadiation,
                           const Tarcog::ISO15099::CBaseLayer & solid);
        void handleNonEnvironmentPreviousLayer(size_t sP,
                                               double hgap_prev,
                                               const Tarcog::ISO15099::CBaseLayer & solid);
        void
          handleEnvironmentPreviousLayer(size_t sP,
                                         const std::shared_ptr<CEnvironment> & previousEnvironment,
                                         double hgap_prev,
                                         const Tarcog::ISO15099::CBaseLayer & solid);
        void handleNonEnvironmentNextLayer(size_t sP,
                                           double hgap_next,
                                           const Tarcog::ISO15099::CBaseLayer & solid);
        void handleEnvironmentNextLayer(size_t sP,
                                        const std::shared_ptr<CEnvironment> & nextEnvironment,
                                        double hgap_next,
                                        const Tarcog::ISO15099::CBaseLayer & solid);
        void buildCell(Tarcog::ISO15099::CBaseLayer & solid, size_t t_Index);

        FenestrationCommon::SquareMatrix m_MatrixA;
        std::vector<double> m_VectorB;

        CIGU & m_IGU;
    };

}   // namespace Tarcog::ISO15099
