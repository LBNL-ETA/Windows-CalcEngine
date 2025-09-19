#pragma once

#include <array>
#include <memory>

#include <WCECommon.hpp>

namespace Tarcog::ISO15099
{
    class Surface;

    // Small wrapper that maps FenestrationCommon::Side → std::array index
    struct SideSurfaceArray
    {
        std::array<std::shared_ptr<Surface>, 2> data{{nullptr, nullptr}};

        static constexpr std::size_t index(FenestrationCommon::Side side) noexcept
        {
            switch(side)
            {
                case FenestrationCommon::Side::Front:
                    return 0;
                case FenestrationCommon::Side::Back:
                    return 1;
            }
            return 0;   // defensive
        }

        std::shared_ptr<Surface> & at(FenestrationCommon::Side side)
        {
            return data.at(index(side));
        }

        [[nodiscard]] const std::shared_ptr<Surface> & at(FenestrationCommon::Side side) const
        {
            return data.at(index(side));
        }

        // Keep existing syntax: m_Surface[Side::Front]
        std::shared_ptr<Surface> & operator[](FenestrationCommon::Side side)
        {
            return data.at(index(side));   // .at() keeps clang-tidy happy
        }
        const std::shared_ptr<Surface> & operator[](FenestrationCommon::Side side) const
        {
            return data.at(index(side));
        }
    };

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

        SideSurfaceArray m_Surface;
    };
}   // namespace Tarcog::ISO15099