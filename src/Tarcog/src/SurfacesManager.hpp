#pragma once

#include <map>
#include <memory>

namespace FenestrationCommon
{
    enum class Side;
}

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

        [[nodiscard]] double getSurfaceTemperature(FenestrationCommon::Side t_Position) const;
        [[nodiscard]] double J(FenestrationCommon::Side t_Position) const;

        virtual double averageLayerTemperature();
        [[nodiscard]] double averageSurfaceTemperature() const;

    protected:
        std::map<FenestrationCommon::Side, std::shared_ptr<Surface>> m_Surface{
          {FenestrationCommon::Side::Front, nullptr}, {FenestrationCommon::Side::Back, nullptr}};
    };
}   // namespace Tarcog::ISO15099