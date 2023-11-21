#include "BaseLayer.hpp"

#include "Surface.hpp"


namespace Tarcog::ISO15099
{
    CBaseLayer::CBaseLayer() : CLayerHeatFlow(), m_PreviousLayer(nullptr), m_NextLayer(nullptr)
    {}

    CBaseLayer::CBaseLayer(double thickness) :
        CLayerHeatFlow(), m_Thickness(thickness), m_PreviousLayer(nullptr), m_NextLayer(nullptr)
    {}

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
        return m_Thickness + getSurface(FenestrationCommon::Side::Front)->getMeanDeflection()
               - getSurface(FenestrationCommon::Side::Back)->getMeanDeflection();
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

    void CBaseLayer::setBaseThickness(const double thickness)
    {
        m_Thickness = thickness;
    }

    double CBaseLayer::getBaseThickness() const
    {
        return m_Thickness;
    }

    double CBaseLayer::getConductivity()
    {
        return getConductionConvectionCoefficient() * m_Thickness;
    }

    double CBaseLayer::getEffectiveThermalConductivity()
    {
        return std::abs(getHeatFlow() * m_Thickness
                        / (getSurfaceTemperature(FenestrationCommon::Side::Front)
                           - getSurfaceTemperature(FenestrationCommon::Side::Back)));
    }
}   // namespace Tarcog::ISO15099
