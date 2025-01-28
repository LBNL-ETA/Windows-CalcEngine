#include "BaseLayer.hpp"

#include "Surface.hpp"


namespace Tarcog::ISO15099
{
    CBaseLayer::CBaseLayer() : HeatFlowLayer(), SurfacesManager()
    {}

    CBaseLayer::CBaseLayer(double thickness) :
        HeatFlowLayer(),
        SurfacesManager(),
        m_Thickness(thickness),
        m_PreviousLayer(nullptr),
        m_NextLayer(nullptr)
    {}

    double CBaseLayer::getHeatFlow()
    {
        return getRadiationFlow() + getConvectionConductionFlow();
    }

    double CBaseLayer::getRadiationFlow()
    {
        calculateRadiationFlow();
        return J(FenestrationCommon::Side::Back) - J(FenestrationCommon::Side::Front);
    }

    double CBaseLayer::getConvectionConductionFlow()
    {
        return (surfaceTemperature(FenestrationCommon::Side::Back)
                - surfaceTemperature(FenestrationCommon::Side::Front))
               * getConductionConvectionCoefficient();
    }

    std::shared_ptr<CBaseLayer> CBaseLayer::getPreviousLayer() const
    {
        return m_PreviousLayer;
    }

    std::shared_ptr<CBaseLayer> CBaseLayer::getNextLayer() const
    {
        return m_NextLayer;
    }

    void CBaseLayer::tearDownConnections()
    {
        m_PreviousLayer = nullptr;
        m_NextLayer = nullptr;
    }

    void CBaseLayer::connectToBackSide(const std::shared_ptr<CBaseLayer> & t_Layer)
    {
        m_NextLayer = t_Layer;
        t_Layer->m_PreviousLayer = shared_from_this();
    }

    void CBaseLayer::calculateRadiationFlow()
    {}

    double CBaseLayer::getThickness() const
    {
        return m_Thickness + surfaceDeflectionMean(FenestrationCommon::Side::Front)
               - surfaceDeflectionMean(FenestrationCommon::Side::Back);
    }

    bool CBaseLayer::isPermeable() const
    {
        return false;
    }

    double CBaseLayer::getSurfaceArea() const
    {
        return m_Width * m_Height;
    }

    void CBaseLayer::setWidth(const double width)
    {
        m_Width = width;
    }

    void CBaseLayer::setHeight(const double height)
    {
        m_Height = height;
    }

    void CBaseLayer::setTilt(const double tilt)
    {
        m_Tilt = tilt;
    }

    double CBaseLayer::getConductivity()
    {
        return getConductionConvectionCoefficient() * getThickness();
    }

    double CBaseLayer::getEffectiveThermalConductivity()
    {
        return std::abs(getHeatFlow() * getThickness()
                        / (surfaceTemperature(FenestrationCommon::Side::Front)
                           - surfaceTemperature(FenestrationCommon::Side::Back)));
    }
}   // namespace Tarcog::ISO15099
