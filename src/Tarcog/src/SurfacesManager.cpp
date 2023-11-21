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

    double SurfacesManager::getSurfaceTemperature(FenestrationCommon::Side t_Position) const
    {
        return m_Surface.at(t_Position)->getTemperature();
    }

    double SurfacesManager::J(const FenestrationCommon::Side t_Position) const
    {
        return m_Surface.at(t_Position)->J();
    }

    double SurfacesManager::averageLayerTemperature()
    {
        return averageSurfaceTemperature();
    }

    double SurfacesManager::averageSurfaceTemperature() const
    {
        return (getSurfaceTemperature(FenestrationCommon::Side::Front)
                + getSurfaceTemperature(FenestrationCommon::Side::Back))
               / 2;
    }
}   // namespace Tarcog::ISO15099