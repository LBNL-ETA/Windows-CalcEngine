#pragma once

#include <vector>

#include "EnvironmentConfigurations.hpp"


namespace Tarcog::ISO15099
{
    enum class System
    {
        Uvalue,
        SHGC
    };

    class IIGUSystem
    {
    public:
        virtual double getUValue() = 0;
        virtual double getSHGC(double t_TotSol) = 0;
        virtual double getHc(System system, Environment environment) const = 0;
        virtual double getHr(System system, Environment environment) const = 0;
        virtual double getH(System system, Environment environment) const = 0;
        virtual double thickness() const = 0;
        virtual std::vector<double> gapLayerThicknesses() const = 0;   // Needed for generic frames
        virtual void setWidth(double width) = 0;
        virtual void setHeight(double height) = 0;
        virtual void setTilt(double tilt) = 0;
        virtual void setWidthAndHeight(double width, double height) = 0;
        virtual void setInteriorAndExteriorSurfacesHeight(double height) = 0;
    };
}   // namespace Tarcog::ISO15099
