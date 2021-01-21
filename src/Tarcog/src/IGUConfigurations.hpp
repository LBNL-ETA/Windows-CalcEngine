#pragma once

#include "EnvironmentConfigurations.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        enum class System
        {
            Uvalue,
            SHGC
        };

        class IIGUSystem
        {
        public:
            virtual double getUValue() const = 0;
            virtual double getSHGC(double t_TotSol) const = 0;
            virtual double getHc(System system, Environment environment) const = 0;
        };
    }
}
