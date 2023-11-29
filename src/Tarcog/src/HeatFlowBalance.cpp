#include "HeatFlowBalance.hpp"
#include "BaseLayer.hpp"
#include "IGUSolidLayer.hpp"
#include "IGU.hpp"
#include "Environment.hpp"
#include "Surface.hpp"


using FenestrationCommon::Side;


namespace Tarcog::ISO15099
{
    CHeatFlowBalance::CHeatFlowBalance(CIGU & t_IGU) :
        m_MatrixA(4 * t_IGU.getNumOfLayers()), m_VectorB(4 * t_IGU.getNumOfLayers()), m_IGU(t_IGU)
    {}

    std::vector<double> CHeatFlowBalance::calcBalanceMatrix()
    {
        auto aSolidLayers = m_IGU.getSolidLayers();
        m_MatrixA.setZeros();
        std::fill(m_VectorB.begin(), m_VectorB.end(), 0);
        for(size_t i = 0; i < aSolidLayers.size(); ++i)
        {
            buildCell(*aSolidLayers[i], i);
        }
        return FenestrationCommon::CLinearSolver::solveSystem(m_MatrixA, m_VectorB);
    }

    void CHeatFlowBalance::buildCell(Tarcog::ISO15099::CBaseLayer & current, const size_t t_Index)
    {
        // Routine is used to build matrix "cell" around solid layer.

        // first determine cell size
        size_t sP = 4 * t_Index;

        auto next = current.getNextLayer();
        auto previous = current.getPreviousLayer();

        // First build base cell
        double hgl = current.getConductionConvectionCoefficient();
        const double hgap_prev = previous->getConductionConvectionCoefficient();
        const double hgap_next = next->getConductionConvectionCoefficient();
        const double qv_prev = previous->getGainFlow();
        const double qv_next = next->getGainFlow();
        const double solarRadiation = current.getGainFlow();

        // first row
        m_MatrixA(sP, sP) = hgap_prev + hgl;
        m_MatrixA(sP, sP + 1) = 1;
        m_MatrixA(sP, sP + 3) = -hgl;
        m_VectorB[sP] += solarRadiation / 2 + qv_prev / 2;

        // second row
        m_MatrixA(sP + 1, sP) = current.emissivePowerTerm(Side::Front);
        m_MatrixA(sP + 1, sP + 1) = -1;

        // third row
        m_MatrixA(sP + 2, sP + 2) = -1;
        m_MatrixA(sP + 2, sP + 3) = current.emissivePowerTerm(Side::Back);

        // fourth row
        m_MatrixA(sP + 3, sP) = hgl;
        m_MatrixA(sP + 3, sP + 2) = -1;
        m_MatrixA(sP + 3, sP + 3) = -hgap_next - hgl;
        m_VectorB[sP + 3] += -solarRadiation / 2 - qv_next / 2;

        if(std::dynamic_pointer_cast<CEnvironment>(previous) == nullptr)
        {
            // first row
            m_MatrixA(sP, sP - 1) = -hgap_prev;
            m_MatrixA(sP, sP - 2) = current.transmittance(Side::Front) - 1;

            // second row
            m_MatrixA(sP + 1, sP - 2) = current.reflectance(Side::Front);

            // third row
            m_MatrixA(sP + 2, sP - 2) = current.transmittance(Side::Front);

            // fourth row
            m_MatrixA(sP + 3, sP - 2) = current.transmittance(Side::Front);
        }
        else
        {
            const double environmentRadiosity =
              std::dynamic_pointer_cast<CEnvironment>(previous)->getEnvironmentIR();
            const double airTemperature =
              std::dynamic_pointer_cast<CEnvironment>(previous)->getGasTemperature();

            m_VectorB[sP] += environmentRadiosity + hgap_prev * airTemperature
                             - environmentRadiosity * current.transmittance(Side::Front);
            m_VectorB[sP + 1] += -current.reflectance(Side::Front) * environmentRadiosity;
            m_VectorB[sP + 2] += -current.transmittance(Side::Front) * environmentRadiosity;
            m_VectorB[sP + 3] += -current.transmittance(Side::Front) * environmentRadiosity;
        }

        if(std::dynamic_pointer_cast<CEnvironment>(next) == nullptr)
        {
            // first row
            m_MatrixA(sP, sP + 5) = -current.transmittance(Side::Back);

            // second row
            m_MatrixA(sP + 1, sP + 5) = current.transmittance(Side::Back);

            // third row
            m_MatrixA(sP + 2, sP + 5) = current.reflectance(Side::Back);

            // fourth row
            m_MatrixA(sP + 3, sP + 4) = hgap_next;
            m_MatrixA(sP + 3, sP + 5) = 1 - current.transmittance(Side::Back);
        }
        else
        {
            const double environmentRadiosity =
              std::dynamic_pointer_cast<CEnvironment>(next)->getEnvironmentIR();
            const double airTemperature =
              std::dynamic_pointer_cast<CEnvironment>(next)->getGasTemperature();

            m_VectorB[sP] += current.transmittance(Side::Back) * environmentRadiosity;
            m_VectorB[sP + 1] += -current.transmittance(Side::Back) * environmentRadiosity;
            m_VectorB[sP + 2] += -current.reflectance(Side::Back) * environmentRadiosity;
            m_VectorB[sP + 3] += -environmentRadiosity - hgap_next * airTemperature
                                 + current.transmittance(Side::Back) * environmentRadiosity;
        }
    }

}   // namespace Tarcog::ISO15099
