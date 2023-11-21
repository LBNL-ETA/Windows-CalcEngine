#include <cassert>
#include <stdexcept>

#include "LayerInterfaces.hpp"
#include "Surface.hpp"
#include "TarcogConstants.hpp"
#include "WCEGases.hpp"
#include "WCECommon.hpp"


using FenestrationCommon::Side;


namespace Tarcog::ISO15099
{
    //////////////////////////////////////////////////////////////////////////
    ///      CLayerHeatFlow
    //////////////////////////////////////////////////////////////////////////

    double CLayerHeatFlow::getGainFlow()
    {
        calculateLayerHeatFlow();
        return m_LayerGainFlow;
    }

    double CLayerHeatFlow::getConductionConvectionCoefficient()
    {
        calculateLayerHeatFlow();
        return m_ConductiveConvectiveCoeff;
    }

    void CLayerHeatFlow::calculateLayerHeatFlow()
    {
        if(!isCalculated())
        {
            calculateRadiationFlow();
            calculateConvectionOrConductionFlow();
        }
        setCalculated();
    }

    void CLayerHeatFlow::resetCalculated()
    {
        m_IsCalculated = false;
    }

    void CLayerHeatFlow::setCalculated()
    {
        m_IsCalculated = true;
    }

    bool CLayerHeatFlow::isCalculated()
    {
        return m_IsCalculated;
    }
}   // namespace Tarcog::ISO15099
