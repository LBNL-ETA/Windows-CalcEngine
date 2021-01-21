#pragma once

#include <memory>
#include <vector>
#include <map>
#include "IGUConfigurations.hpp"

namespace Tarcog
{
    namespace ISO15099
    {
        enum class Environment;

        class CIGU;

        class CEnvironment;

        class CSingleSystem;

        class CIGUSolidLayer;

        class CSystem : public IIGUSystem
        {
        public:
            CSystem(CIGU & t_IGU,
                    const std::shared_ptr<CEnvironment> & t_Indoor,
                    const std::shared_ptr<CEnvironment> & t_Outdoor);

            std::vector<double> getTemperatures(System t_System) const;
            std::vector<double> getRadiosities(System t_System) const;

            std::vector<double> getMaxDeflections(System t_System) const;
            std::vector<double> getMeanDeflections(System t_System) const;

            std::vector<std::shared_ptr<CIGUSolidLayer>> getSolidLayers(System t_System) const;

            std::vector<double> getSolidEffectiveLayerConductivities(const System t_System) const;
            std::vector<double> getGapEffectiveLayerConductivities(const System t_System) const;
            double getEffectiveSystemConductivity(const System t_System) const;
            double thickness(const System t_System) const;

            double getHeatFlow(System t_System, Environment t_Environment) const;
            double getUValue() const override;
            double getSHGC(double t_TotSol) const override;
            double getHc(System sys, Environment environment) const override;
            size_t getNumberOfIterations(System t_System) const;

            double relativeHeatGain(double Tsol) const;

            void setAbsorptances(const std::vector<double> & absorptances);

        private:
            std::map<System, std::shared_ptr<CSingleSystem>> m_System;
        };

    }   // namespace ISO15099

}   // namespace Tarcog
