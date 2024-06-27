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
        return FenestrationCommon::solveSystem(m_MatrixA, m_VectorB);
    }

    double getConductionConvectionCoefficient(const std::shared_ptr<CBaseLayer> & layer)
    {
        return layer ? layer->getConductionConvectionCoefficient() : 0.0;
    }

    double getGainFlow(const std::shared_ptr<CBaseLayer> & layer)
    {
        return layer ? layer->getGainFlow() : 0.0;
    }

    void CHeatFlowBalance::buildBaseCell(size_t sP,
                                         double hgl,
                                         double hgap_prev,
                                         double hgap_next,
                                         double qv_prev,
                                         double qv_next,
                                         double solarRadiation,
                                         const Tarcog::ISO15099::CBaseLayer & solid)
    {
        // first row
        m_MatrixA(sP, sP) = hgap_prev + hgl;
        m_MatrixA(sP, sP + 1) = 1;
        m_MatrixA(sP, sP + 3) = -hgl;
        m_VectorB[sP] += solarRadiation / 2 + qv_prev / 2;

        // second row
        m_MatrixA(sP + 1, sP) = solid.emissivePowerTerm(Side::Front);
        m_MatrixA(sP + 1, sP + 1) = -1;

        // third row
        m_MatrixA(sP + 2, sP + 2) = -1;
        m_MatrixA(sP + 2, sP + 3) = solid.emissivePowerTerm(Side::Back);

        // fourth row
        m_MatrixA(sP + 3, sP) = hgl;
        m_MatrixA(sP + 3, sP + 2) = -1;
        m_MatrixA(sP + 3, sP + 3) = -hgap_next - hgl;
        m_VectorB[sP + 3] += -solarRadiation / 2 - qv_next / 2;
    }

    void CHeatFlowBalance::handleNonEnvironmentPreviousLayer(
      size_t sP, double hgap_prev, const Tarcog::ISO15099::CBaseLayer & solid)
    {
        // first row
        m_MatrixA(sP, sP - 1) = -hgap_prev;
        m_MatrixA(sP, sP - 2) = solid.transmittance(Side::Front) - 1;

        // second row
        m_MatrixA(sP + 1, sP - 2) = solid.reflectance(Side::Front);

        // third row
        m_MatrixA(sP + 2, sP - 2) = solid.transmittance(Side::Front);

        // fourth row
        m_MatrixA(sP + 3, sP - 2) = solid.transmittance(Side::Front);
    }

    void CHeatFlowBalance::handleEnvironmentPreviousLayer(
      size_t sP,
      const std::shared_ptr<CEnvironment> & previousEnvironment,
      double hgap_prev,
      const Tarcog::ISO15099::CBaseLayer & solid)
    {
        double environmentRadiosity = previousEnvironment->getEnvironmentIR();
        double airTemperature = previousEnvironment->getGasTemperature();

        m_VectorB[sP] += environmentRadiosity + hgap_prev * airTemperature
                         - environmentRadiosity * solid.transmittance(Side::Front);
        m_VectorB[sP + 1] += -solid.reflectance(Side::Front) * environmentRadiosity;
        m_VectorB[sP + 2] += -solid.transmittance(Side::Front) * environmentRadiosity;
        m_VectorB[sP + 3] += -solid.transmittance(Side::Front) * environmentRadiosity;
    }

    void CHeatFlowBalance::handleNonEnvironmentNextLayer(size_t sP,
                                                         double hgap_next,
                                                         const Tarcog::ISO15099::CBaseLayer & solid)
    {
        // first row
        m_MatrixA(sP, sP + 5) = -solid.transmittance(Side::Back);

        // second row
        m_MatrixA(sP + 1, sP + 5) = solid.transmittance(Side::Back);

        // third row
        m_MatrixA(sP + 2, sP + 5) = solid.reflectance(Side::Back);

        // fourth row
        m_MatrixA(sP + 3, sP + 4) = hgap_next;
        m_MatrixA(sP + 3, sP + 5) = 1 - solid.transmittance(Side::Back);
    }

    void CHeatFlowBalance::handleEnvironmentNextLayer(
      size_t sP,
      const std::shared_ptr<CEnvironment> & nextEnvironment,
      double hgap_next,
      const Tarcog::ISO15099::CBaseLayer & solid)
    {
        double environmentRadiosity = nextEnvironment->getEnvironmentIR();
        double airTemperature = nextEnvironment->getGasTemperature();

        m_VectorB[sP] += solid.transmittance(Side::Back) * environmentRadiosity;
        m_VectorB[sP + 1] += -solid.transmittance(Side::Back) * environmentRadiosity;
        m_VectorB[sP + 2] += -solid.reflectance(Side::Back) * environmentRadiosity;
        m_VectorB[sP + 3] += -environmentRadiosity - hgap_next * airTemperature
                             + solid.transmittance(Side::Back) * environmentRadiosity;
    }


    void CHeatFlowBalance::buildCell(Tarcog::ISO15099::CBaseLayer & solid, const size_t t_Index)
    {
        // Routine is used to build matrix "cell" around solid layer.
        size_t sP = 4 * t_Index;

        auto next = solid.getNextLayer();
        auto previous = solid.getPreviousLayer();

        auto hgl = solid.getConductionConvectionCoefficient();
        auto hgap_prev = getConductionConvectionCoefficient(previous);
        auto hgap_next = getConductionConvectionCoefficient(next);
        auto qv_prev = getGainFlow(previous);
        auto qv_next = getGainFlow(next);
        auto solarRadiation = solid.getGainFlow();

        buildBaseCell(sP, hgl, hgap_prev, hgap_next, qv_prev, qv_next, solarRadiation, solid);

        auto previousEnvironment = std::dynamic_pointer_cast<CEnvironment>(previous);
        auto nextEnvironment = std::dynamic_pointer_cast<CEnvironment>(next);

        if(!previousEnvironment)
        {
            handleNonEnvironmentPreviousLayer(sP, hgap_prev, solid);
        }
        else
        {
            handleEnvironmentPreviousLayer(sP, previousEnvironment, hgap_prev, solid);
        }

        if(!nextEnvironment)
        {
            handleNonEnvironmentNextLayer(sP, hgap_next, solid);
        }
        else
        {
            handleEnvironmentNextLayer(sP, nextEnvironment, hgap_next, solid);
        }
    }


}   // namespace Tarcog::ISO15099
