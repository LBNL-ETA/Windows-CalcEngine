#include <WCECommon.hpp>

#include "SurfacesManager.hpp"
#include "Surface.hpp"

namespace Tarcog::ISO15099
{
    SurfacesManager::SurfacesManager(const SurfacesManager & t_Layer)
    {
        operator=(t_Layer);
    }

    SurfacesManager & SurfacesManager::operator=(const SurfacesManager & t_Layer)
    {
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

    std::shared_ptr<Surface>
      SurfacesManager::getSurface(const FenestrationCommon::Side t_Position) const
    {
        return m_Surface.at(t_Position);
    }

    void SurfacesManager::setSurface(std::shared_ptr<Surface> t_Surface,
                                     const FenestrationCommon::Side t_Position)
    {
        m_Surface[t_Position] = t_Surface;
    }

    double SurfacesManager::surfaceTemperature(FenestrationCommon::Side t_Position) const
    {
        return m_Surface.at(t_Position)->getTemperature();
    }

    double SurfacesManager::J(const FenestrationCommon::Side t_Position) const
    {
        return m_Surface.at(t_Position)->J();
    }

    double SurfacesManager::surfaceDeflectionMax(FenestrationCommon::Side side) const
    {
        return m_Surface.at(side)->getMaxDeflection();
    }

    double SurfacesManager::surfaceDeflectionMean(FenestrationCommon::Side side) const
    {
        return m_Surface.at(side)->getMeanDeflection();
    }

    double SurfacesManager::averageLayerTemperature()
    {
        return averageSurfaceTemperature();
    }

    double SurfacesManager::averageSurfaceTemperature() const
    {
        return (surfaceTemperature(FenestrationCommon::Side::Front)
                + surfaceTemperature(FenestrationCommon::Side::Back))
               / 2;
    }

    double SurfacesManager::emissivePowerTerm(FenestrationCommon::Side side) const
    {
        return m_Surface.at(side)->emissivePowerTerm();
    }

    double SurfacesManager::reflectance(FenestrationCommon::Side side) const
    {
        return m_Surface.at(side)->getReflectance();
    }

    double SurfacesManager::transmittance(FenestrationCommon::Side side) const
    {
        return m_Surface.at(side)->getTransmittance();
    }

    void SurfacesManager::initializeStart(FenestrationCommon::Side side, double temperature)
    {
        m_Surface.at(side)->initializeStart(temperature);
    }
}   // namespace Tarcog::ISO15099