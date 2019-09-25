#ifndef TARCOGSYSTEM_H
#define TARCOGSYSTEM_H

#include <memory>
#include <vector>
#include <map>

namespace Tarcog
{
    namespace ISO15099
    {
        enum class Environment;

        class CIGU;

        class CEnvironment;

        class CSingleSystem;

        class CIGUSolidLayer;

        enum class System
        {
            Uvalue,
            SHGC
        };

        class CSystem
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

            double getHeatFlow(System t_System, Environment t_Environment) const;
            double getUValue() const;
            double getSHGC(double t_TotSol) const;
            size_t getNumberOfIterations(System t_System) const;

        private:
            std::map<System, std::shared_ptr<CSingleSystem>> m_System;
        };

    }   // namespace ISO15099

}   // namespace Tarcog

#endif
