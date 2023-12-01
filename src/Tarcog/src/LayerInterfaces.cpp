#include "LayerInterfaces.hpp"


namespace Tarcog::ISO15099
{
    //////////////////////////////////////////////////////////////////////////
    ///      CLayerHeatFlow
    //////////////////////////////////////////////////////////////////////////

    double HeatFlowLayer::getGainFlow()
    {
        calculateLayerHeatFlow();
        return m_LayerGainFlow;
    }

    double HeatFlowLayer::getConductionConvectionCoefficient()
    {
        calculateLayerHeatFlow();
        return m_ConductiveConvectiveCoeff;
    }

    void HeatFlowLayer::calculateLayerHeatFlow()
    {
        if(!isCalculated())
        {
            calculateRadiationFlow();
            calculateConvectionOrConductionFlow();
        }
        setCalculated();
    }

    void HeatFlowLayer::resetCalculated()
    {
        m_IsCalculated = false;
    }

    void HeatFlowLayer::setCalculated()
    {
        m_IsCalculated = true;
    }

    bool HeatFlowLayer::isCalculated()
    {
        return m_IsCalculated;
    }
}   // namespace Tarcog::ISO15099
