#include "BaseLayer.hpp"
#include "Surface.hpp"

namespace Tarcog::ISO15099
{
    CBaseLayer::CBaseLayer() : HeatFlowLayer() {}

    CBaseLayer::CBaseLayer(double thickness)
        : HeatFlowLayer(), m_Thickness(thickness)
    {
        if (thickness <= 0)
            throw std::runtime_error("Layer thickness must be greater than zero.");
    }

    CBaseLayer::~CBaseLayer()
    {
        tearDownConnections();
    }

    std::shared_ptr<CBaseLayer> CBaseLayer::getPreviousLayer() const
    {
        return m_PreviousLayer.lock();
    }

    std::shared_ptr<CBaseLayer> CBaseLayer::getNextLayer() const
    {
        return m_NextLayer.lock();
    }

    void CBaseLayer::connectToBackSide(const std::shared_ptr<CBaseLayer> & t_Layer)
    {
        m_NextLayer = t_Layer;
        try {
            auto self = shared_from_this();
            t_Layer->m_PreviousLayer = self;
        } catch (const std::bad_weak_ptr&) {
            // Defensive: if this layer isn't managed by shared_ptr yet.
            t_Layer->m_PreviousLayer.reset();
        }
    }

    void CBaseLayer::tearDownConnections()
    {
        m_NextLayer.reset();
        m_PreviousLayer.reset();
    }

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

    void CBaseLayer::calculateRadiationFlow() {}

    double CBaseLayer::getThickness() const
    {
        return m_Thickness + surfaceDeflectionMean(FenestrationCommon::Side::Front)
               - surfaceDeflectionMean(FenestrationCommon::Side::Back);
    }

    bool CBaseLayer::isPermeable() const { return false; }

    double CBaseLayer::getSurfaceArea() const { return m_Width * m_Height; }

    void CBaseLayer::setWidth(double width) { m_Width = width; }
    void CBaseLayer::setHeight(double height) { m_Height = height; }
    void CBaseLayer::setTilt(double tilt) { m_Tilt = tilt; }

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
}
