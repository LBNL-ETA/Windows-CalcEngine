#pragma once

#include <memory>
#include <vector>
#include <map>
#include "IGUConfigurations.hpp"
#include "IGUGapLayer.hpp"
#include "ShadingModifiers.hpp"


namespace Tarcog::ISO15099
{
    enum class Environment;

    class CIGU;

    class CEnvironment;

    class CSingleSystem;

    class CIGUSolidLayer;

    class CSystem : public IIGUSystem
    {
    public:
        virtual ~CSystem() = default;
        CSystem(CIGU & t_IGU,
                const std::shared_ptr<CEnvironment> & t_Indoor,
                const std::shared_ptr<CEnvironment> & t_Outdoor);

        [[nodiscard]] std::vector<double> getTemperatures(System t_System);
        [[nodiscard]] std::vector<double> getRadiosities(System t_System);

        [[nodiscard]] std::vector<double> getMaxLayerDeflections(System t_System);
        [[nodiscard]] std::vector<double> getMeanLayerDeflections(System t_System);
        [[nodiscard]] std::vector<double> getMaxGapWidth(System t_System);
        [[nodiscard]] std::vector<double> getMeanGapWidth(System t_System);
        [[nodiscard]] std::vector<double> getPanesLoad(System t_System);
        void setAppliedLoad(const std::vector<double> & load);

        [[nodiscard]] std::vector<double> getGapPressures(System t_System);

        [[nodiscard]] std::vector<std::shared_ptr<CIGUSolidLayer>>
          getSolidLayers(System t_System) const;

        [[nodiscard]] std::vector<std::shared_ptr<CIGUGapLayer>>
          getGapLayers(System t_System) const;

        [[nodiscard]] std::vector<double> getSolidEffectiveLayerConductivities(System t_System);
        [[nodiscard]] std::vector<double> getGapEffectiveLayerConductivities(System t_System);
        [[nodiscard]] double getEffectiveSystemConductivity(System t_System);
        [[nodiscard]] double thickness(System t_System) const;
        [[nodiscard]] double thickness() const override;
        [[nodiscard]] std::vector<double> solidLayerThicknesses() const;
        [[nodiscard]] std::vector<double> gapLayerThicknesses() const override;

        [[nodiscard]] double getHeatFlow(System t_System, Environment t_Environment);
        [[nodiscard]] double getUValue() override;
        [[nodiscard]] double getSHGC(double t_TotSol) override;
        [[nodiscard]] double getHc(System sys, Environment environment) const override;
        [[nodiscard]] double getHr(System sys, Environment environment) const override;
        [[nodiscard]] double getH(System sys, Environment environment) const override;
        [[nodiscard]] size_t getNumberOfIterations(System t_System);

        [[nodiscard]] double relativeHeatGain(double Tsol);

        void setAbsorptances(const std::vector<double> & absorptances);

        void setWidth(double width) override;
        void setHeight(double height) override;
        void setTilt(double tilt) override;
        void setWidthAndHeight(double width, double height) override;
        void setInteriorAndExteriorSurfacesHeight(double height) override;

        void setDeflectionProperties(double t_Tini, double t_Pini);
        void setDeflectionProperties(const std::vector<double> & measuredGapWidths);
        void clearDeflection();

        void setSolidLayerConductivities(
          const std::vector<double> & t_SolidLayerThermalConductivities);
        void setSolidLayerConductivity(size_t t_LayerIndex, double t_SolidLayerThermalConductivity);

        [[nodiscard]] ShadingModifier getShadingModifier(System t_System, Environment env) const;

    private:
        void solve();
        void checkSolved();

        std::map<System, std::shared_ptr<CSingleSystem>> m_System;

        bool m_Solved{false};
    };

}   // namespace Tarcog::ISO15099
