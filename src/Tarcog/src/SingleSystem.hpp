#pragma once

#include <memory>
#include <map>
#include <vector>

#include "IGU.hpp"

#include "ShadingModifiers.hpp"


namespace Tarcog::ISO15099
{
    enum class Environment;

    class CIGUSolidLayer;

    class CIGUGapLayer;

    class CEnvironment;

    class CNonLinearSolver;

    class CSingleSystem
    {
    public:
        CSingleSystem(CIGU & t_IGU,
                      const std::shared_ptr<CEnvironment> & t_Indoor,
                      const std::shared_ptr<CEnvironment> & t_Outdoor);

        CSingleSystem(const CSingleSystem & t_SingleSystem);
        CSingleSystem & operator=(const CSingleSystem & t_SingleSystem);

        [[nodiscard]] std::vector<std::shared_ptr<CIGUSolidLayer>> getSolidLayers() const;
        [[nodiscard]] std::vector<std::shared_ptr<CIGUGapLayer>> getGapLayers() const;

        [[nodiscard]] std::vector<double> getSolidEffectiveLayerConductivities() const;
        [[nodiscard]] std::vector<double> getGapEffectiveLayerConductivities() const;

        [[nodiscard]] std::vector<double> getTemperatures() const;
        [[nodiscard]] std::vector<double> getRadiosities() const;

        [[nodiscard]] std::vector<double> getMaxLayerDeflections() const;
        [[nodiscard]] std::vector<double> getMeanLayerDeflections() const;
        [[nodiscard]] std::vector<double> getMaxGapWidth() const;
        [[nodiscard]] std::vector<double> getMeanGapWidth() const;
        [[nodiscard]] std::vector<double> getPanesLoad();
        void setAppliedLoad(std::vector<double> load);

        [[nodiscard]] std::vector<double> getGapPressures() const;

        [[nodiscard]] std::shared_ptr<CSingleSystem> clone() const;

        [[nodiscard]] double getHeatFlow(Environment t_Environment) const;
        [[nodiscard]] double getConvectiveHeatFlow(Environment t_Environment) const;
        [[nodiscard]] double getRadiationHeatFlow(Environment t_Environment) const;
        [[nodiscard]] double getHc(Environment t_Environment) const;
        [[nodiscard]] double getHr(Environment t_Environment) const;
        [[nodiscard]] double getH(Environment t_Environment) const;
        [[nodiscard]] double getAirTemperature(Environment t_Environment) const;

        // If interior layer have openings, this will return heat flow from airflow
        [[nodiscard]] double getVentilationFlow(Environment t_Environment) const;
        [[nodiscard]] double getUValue() const;
        [[nodiscard]] size_t getNumberOfIterations() const;
        [[nodiscard]] double solutionTolarance() const;
        [[nodiscard]] bool isToleranceAchieved() const;

        [[nodiscard]] double EffectiveConductivity() const;

        // Set solution tolerance
        void setTolerance(double t_Tolerance) const;
        // Set intial guess for solution.
        void setInitialGuess(const std::vector<double> & t_Temperatures) const;

        void setSolarRadiation(double t_SolarRadiation);
        [[nodiscard]] double getSolarRadiation() const;

        void solve();

        [[nodiscard]] double thickness() const;

        void setAbsorptances(const std::vector<double> & absorptances);

        void setWidth(double width);
        void setHeight(double height);
        void setTilt(double tilt);

        //! If IGU is part of the window then frame will still count in surface height.
        void setInteriorAndExteriorSurfacesHeight(double height);

        void setDeflectionProperties(double t_Tini, double t_Pini);
        void setDeflectionProperties(const std::vector<double> & measuredGapWidths);

        void clearDeflection();

        void setSolidLayerConductivities(
          const std::vector<double> & t_SolidLayerThermalConductivities);
        void setSolidLayerConductivity(size_t t_LayerIndex, double t_SolidLayerThermalConductivity);

        ShadingModifier getShadingModifier(Tarcog::ISO15099::Environment environment) const;

    private:
        CIGU m_IGU;
        std::map<Environment, std::shared_ptr<CEnvironment>> m_Environment;
        std::shared_ptr<CNonLinearSolver> m_NonLinearSolver;
        void initializeStartValues();

        // Helper enum use in evaluation of the shading modifiers
        enum class ShadePosition
        {
            Interior,
            Exterior
        };

        [[nodiscard]] ShadingModifier calculateShadingModifier(ShadePosition position) const;
        [[nodiscard]] ShadingModifiers calculateShadingModifiers() const;

        ShadingModifiers m_ShadingModifiers;
    };

}   // namespace Tarcog::ISO15099
