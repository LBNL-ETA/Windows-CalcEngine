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

    CLayerHeatFlow::CLayerHeatFlow() : m_ConductiveConvectiveCoeff(0), m_LayerGainFlow(0)
    {
        m_Surface[Side::Front] = nullptr;
        m_Surface[Side::Back] = nullptr;
    }

    CLayerHeatFlow::CLayerHeatFlow(CLayerHeatFlow const & t_Layer)
    {
        operator=(t_Layer);
    }

    CLayerHeatFlow & CLayerHeatFlow::operator=(CLayerHeatFlow const & t_Layer)
    {
        m_ConductiveConvectiveCoeff = t_Layer.m_ConductiveConvectiveCoeff;
        m_LayerGainFlow = t_Layer.m_LayerGainFlow;
        for(auto aSide : FenestrationCommon::EnumSide())
        {
            const auto aSurface = t_Layer.m_Surface.at(aSide);
            if(aSurface != nullptr)
            {
                m_Surface[aSide] = aSurface->clone();
            }
        }

        return *this;
    }

    double CLayerHeatFlow::getHeatFlow()
    {
        return getRadiationFlow() + getConvectionConductionFlow();
    }

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

    double CLayerHeatFlow::getRadiationFlow()
    {
        calculateRadiationFlow();
        assert(m_Surface.at(Side::Front) != nullptr);
        assert(m_Surface.at(Side::Back) != nullptr);
        return m_Surface.at(Side::Back)->J() - m_Surface.at(Side::Front)->J();
    }

    double CLayerHeatFlow::getConvectionConductionFlow()
    {
        calculateLayerHeatFlow();
        assert(m_Surface.at(Side::Front) != nullptr);
        assert(m_Surface.at(Side::Back) != nullptr);
        return (m_Surface.at(Side::Back)->getTemperature()
                - m_Surface.at(Side::Front)->getTemperature())
               * m_ConductiveConvectiveCoeff;
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

    bool CLayerHeatFlow::areSurfacesInitialized() const
    {
        auto areInitialized = (m_Surface.size() == 2);
        if(areInitialized)
        {
            areInitialized =
              m_Surface.at(Side::Front) != nullptr && m_Surface.at(Side::Back) != nullptr;
        }
        return areInitialized;
    }

    std::shared_ptr<Surface> CLayerHeatFlow::getSurface(Side const t_Position) const
    {
        return m_Surface.at(t_Position);
    }

    void CLayerHeatFlow::setSurface(std::shared_ptr<Surface> t_Surface, Side const t_Position)
    {
        m_Surface[t_Position] = t_Surface;
        if(m_Surface.size() == 2)
        {
            resetCalculated();
        }
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
