#pragma once

#include "IGUConfigurations.hpp"


namespace Tarcog::ISO15099
{
    class SimpleIGU : public IIGUSystem
    {
    public:
        virtual ~SimpleIGU() = default;

        // Fule of five
        SimpleIGU() = default;
        SimpleIGU(const SimpleIGU &) = default;
        SimpleIGU & operator=(const SimpleIGU &) = default;
        SimpleIGU(SimpleIGU &&) noexcept = default;
        SimpleIGU & operator=(SimpleIGU &&) noexcept = default;

        SimpleIGU(double uValue,
                  double shgc,
                  double h,
                  double thickness = 0,
                  std::vector<double> gapLayerThicknesses = {});

        double getUValue() override;
        double getSHGC(double t_TotSol) override;
        [[nodiscard]] double getHc(System system, Environment environment) const override;
        [[nodiscard]] double getHr(System system, Environment environment) const override;
        [[nodiscard]] double getH(System system, Environment environment) const override;

        void setWidth(double width) override;
        void setHeight(double height) override;
        void setTilt(double tilt) override;
        void setWidthAndHeight(double width, double height) override;
        void setInteriorAndExteriorSurfacesHeight(double height) override;
        void setTemperatures(std::vector<double> temperatures);

        [[nodiscard]] double thickness() const override;
        [[nodiscard]] std::vector<double> getTemperatures(System t_System) override;
        [[nodiscard]] std::vector<double> gapLayerThicknesses() const override;

    private:
        double m_UValue{0};
        double m_SHGC{0};
        double m_H{0};
        double m_Thickness{0};
        std::vector<double> m_gapLayerThicknesses;
        std::vector<double> m_Temperatures;
    };
}   // namespace Tarcog::ISO15099
