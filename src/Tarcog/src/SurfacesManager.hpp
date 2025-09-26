#pragma once

#include <array>
#include <memory>

#include <WCECommon.hpp>

namespace Tarcog::ISO15099
{
    class Surface;

    class SurfacesManager
    {
    public:
        SurfacesManager() = default;
        SurfacesManager(const SurfacesManager & t_Layer);
        SurfacesManager & operator=(const SurfacesManager & t_Layer);

        [[nodiscard]] virtual std::shared_ptr<Surface>
          getSurface(FenestrationCommon::Side t_Position) const final;
        virtual void setSurface(std::shared_ptr<Surface> t_Surface,
                                FenestrationCommon::Side t_Position) final;

        [[nodiscard]] double surfaceTemperature(FenestrationCommon::Side t_Position) const;
        [[nodiscard]] double J(FenestrationCommon::Side t_Position) const;
        [[nodiscard]] double surfaceDeflectionMax(FenestrationCommon::Side side) const;
        [[nodiscard]] double surfaceDeflectionMean(FenestrationCommon::Side side) const;

        virtual double averageLayerTemperature();
        [[nodiscard]] double averageSurfaceTemperature() const;

        [[nodiscard]] double emissivePowerTerm(FenestrationCommon::Side side) const;

        [[nodiscard]] double reflectance(FenestrationCommon::Side side) const;
        [[nodiscard]] double transmittance(FenestrationCommon::Side side) const;

        void initializeStart(FenestrationCommon::Side side, double temperature);

    protected:
        void setSurfaceState(double t_Temperature, double t_J, FenestrationCommon::Side t_Position);

        util::EnumArray<FenestrationCommon::Side, std::shared_ptr<Surface>> m_Surface;
    };
}   // namespace Tarcog::ISO15099