#include <memory>
#include <vector>
#include <ranges>
#include <stdexcept>
#include <cassert>

#include "SingleSystem.hpp"
#include "IGUSolidLayer.hpp"
#include "IGUGapLayer.hpp"
#include "IGU.hpp"
#include "OutdoorEnvironment.hpp"
#include "Surface.hpp"
#include "NonLinearSolver.hpp"


using FenestrationCommon::Side;


namespace Tarcog::ISO15099
{
    CSingleSystem::CSingleSystem(CIGU & t_IGU,
                                 std::shared_ptr<CEnvironment> const & t_Indoor,
                                 std::shared_ptr<CEnvironment> const & t_Outdoor) :
        m_IGU(t_IGU)
    {
        m_Environment[Environment::Indoor] = t_Indoor;
        m_Environment[Environment::Outdoor] = t_Outdoor;


        if(t_Indoor == nullptr)
        {
            throw std::runtime_error(
              "Indoor environment has not been assigned to the system. Null value passed.");
        }

        if(t_Outdoor == nullptr)
        {
            throw std::runtime_error(
              "Outdoor environment has not been assigned to the system. Null value passed.");
        }

        const auto aIndoorLayer = m_IGU.getEnvironment(Environment::Indoor);
        auto aIndoor = m_Environment.at(Environment::Indoor);
        aIndoor->connectToIGULayer(aIndoorLayer);
        aIndoor->setTilt(m_IGU.getTilt());
        aIndoor->setWidth(m_IGU.getWidth());
        aIndoor->setHeight(m_IGU.getHeight());

        const auto aOutdoorLayer = m_IGU.getEnvironment(Environment::Outdoor);
        auto aOutdoor = m_Environment.at(Environment::Outdoor);
        aOutdoor->connectToIGULayer(aOutdoorLayer);
        aOutdoor->setTilt(m_IGU.getTilt());
        aOutdoor->setWidth(m_IGU.getWidth());
        aOutdoor->setHeight(m_IGU.getHeight());

        const auto solarRadiation = t_Outdoor->getDirectSolarRadiation();
        m_IGU.setSolarRadiation(solarRadiation);

        initializeStartValues();

        m_NonLinearSolver = std::make_shared<CNonLinearSolver>(m_IGU);
    }

    CSingleSystem::CSingleSystem(const CSingleSystem & t_SingleSystem) : m_IGU(t_SingleSystem.m_IGU)
    {
        operator=(t_SingleSystem);
    }

    CSingleSystem & CSingleSystem::operator=(CSingleSystem const & t_SingleSystem)
    {
        m_IGU = t_SingleSystem.m_IGU;
        m_Environment[Environment::Indoor] =
          t_SingleSystem.m_Environment.at(Environment::Indoor)->cloneEnvironment();
        const auto aLastLayer = m_IGU.getEnvironment(Environment::Indoor);
        m_Environment.at(Environment::Indoor)->connectToIGULayer(aLastLayer);

        m_Environment[Environment::Outdoor] =
          t_SingleSystem.m_Environment.at(Environment::Outdoor)->cloneEnvironment();
        const auto aFirstLayer = m_IGU.getEnvironment(Environment::Outdoor);
        m_Environment.at(Environment::Outdoor)->connectToIGULayer(aFirstLayer);

        // initializeStartValues();

        m_NonLinearSolver =
          std::make_shared<CNonLinearSolver>(m_IGU, t_SingleSystem.getNumberOfIterations());

        return *this;
    }

    std::vector<std::shared_ptr<CIGUSolidLayer>> CSingleSystem::getSolidLayers() const
    {
        return m_IGU.getSolidLayers();
    }

    std::vector<std::shared_ptr<CIGUGapLayer>> CSingleSystem::getGapLayers() const
    {
        return m_IGU.getGapLayers();
    }

    std::vector<double> CSingleSystem::getTemperatures() const
    {
        return m_IGU.getTemperatures();
    }

    std::vector<double> CSingleSystem::getRadiosities() const
    {
        return m_IGU.getRadiosities();
    }

    std::vector<double> CSingleSystem::getMaxLayerDeflections() const
    {
        return m_IGU.getMaxLayerDeflections();
    }

    std::vector<double> CSingleSystem::getMeanLayerDeflections() const
    {
        return m_IGU.getMeanLayerDeflections();
    }

    std::vector<double> CSingleSystem::getMaxGapWidth() const
    {
        return m_IGU.getMaxGapWidth();
    }

    std::vector<double> CSingleSystem::getMeanGapWidth() const
    {
        return m_IGU.getMeanGapWidth();
    }

    std::vector<double> CSingleSystem::getPanesLoad()
    {
        return m_IGU.getPanesLoad();
    }

    void CSingleSystem::setAppliedLoad(std::vector<double> load)
    {
        m_IGU.setAppliedLoad(std::move(load));
    }

    std::shared_ptr<CSingleSystem> CSingleSystem::clone() const
    {
        return std::make_shared<CSingleSystem>(*this);
    }

    double CSingleSystem::getHeatFlow(Environment const t_Environment) const
    {
        return m_Environment.at(t_Environment)->getHeatFlow();
    }

    double CSingleSystem::getConvectiveHeatFlow(Environment const t_Environment) const
    {
        return m_Environment.at(t_Environment)->getConvectionConductionFlow();
    }

    double CSingleSystem::getRadiationHeatFlow(Environment const t_Environment) const
    {
        return m_Environment.at(t_Environment)->getRadiationFlow();
    }

    double CSingleSystem::getHc(Environment const t_Environment) const
    {
        return m_Environment.at(t_Environment)->getHc();
    }

    double CSingleSystem::getHr(Environment t_Environment) const
    {
        return m_Environment.at(t_Environment)->getHr();
        ;
    }

    double CSingleSystem::getH(Environment t_Environment) const
    {
        return getHc(t_Environment) + getHr(t_Environment);
    }

    double CSingleSystem::getAirTemperature(Environment const t_Environment) const
    {
        return m_Environment.at(t_Environment)->getAirTemperature();
    }

    double CSingleSystem::getVentilationFlow(Environment const t_Environment) const
    {
        return m_IGU.getVentilationFlow(t_Environment);
    }

    double CSingleSystem::getUValue() const
    {
        const double interiorAirTemperature =
          m_Environment.at(Environment::Indoor)->getAmbientTemperature();
        const double outdoorAirTemperature =
          m_Environment.at(Environment::Outdoor)->getAmbientTemperature();

        const auto ventilatedFlow{getVentilationFlow(Environment::Indoor)};

        return (getHeatFlow(Environment::Indoor) + ventilatedFlow)
               / (interiorAirTemperature - outdoorAirTemperature);
    }

    void CSingleSystem::setTolerance(double const t_Tolerance) const
    {
        assert(m_NonLinearSolver != nullptr);
        m_NonLinearSolver->setTolerance(t_Tolerance);
    }

    size_t CSingleSystem::getNumberOfIterations() const
    {
        assert(m_NonLinearSolver != nullptr);
        return m_NonLinearSolver->getNumOfIterations();
    }

    double CSingleSystem::solutionTolarance() const
    {
        assert(m_NonLinearSolver != nullptr);
        return m_NonLinearSolver->solutionTolerance();
    }

    bool CSingleSystem::isToleranceAchieved() const
    {
        assert(m_NonLinearSolver != nullptr);
        return m_NonLinearSolver->isToleranceAchieved();
    }

    void CSingleSystem::solve() const
    {
        assert(m_NonLinearSolver != nullptr);
        m_NonLinearSolver->solve();
    }

    void CSingleSystem::initializeStartValues()
    {
        auto const startX = 0.001;
        const auto thickness = m_IGU.getThickness() + startX + 0.01;
        const auto tOut = m_Environment.at(Environment::Outdoor)->getAirTemperature();
        const auto tInd = m_Environment.at(Environment::Indoor)->getAirTemperature();

        const auto deltaTemp = (tInd - tOut) / thickness;

        auto aLayers = m_IGU.getLayers();

        const auto aLayer = aLayers.front();
        auto currentXPosition = startX;
        auto curTemp = tOut + currentXPosition * deltaTemp;

        aLayer->initializeStart(Side::Front, curTemp);

        for(const auto & layer : aLayers)
        {
            currentXPosition += layer->getThickness();
            curTemp = tOut + currentXPosition * deltaTemp;
            layer->initializeStart(Side::Back, curTemp);
        }
    }

    void CSingleSystem::setInitialGuess(const std::vector<double> & t_Temperatures) const
    {
        m_IGU.setInitialGuess(t_Temperatures);
    }

    void CSingleSystem::setSolarRadiation(double const t_SolarRadiation)
    {
        std::dynamic_pointer_cast<COutdoorEnvironment>(m_Environment.at(Environment::Outdoor))
          ->setSolarRadiation(t_SolarRadiation);
        m_IGU.setSolarRadiation(t_SolarRadiation);
    }

    double CSingleSystem::getSolarRadiation() const
    {
        return std::dynamic_pointer_cast<COutdoorEnvironment>(
                 m_Environment.at(Environment::Outdoor))
          ->getSolarRadiation();
    }

    std::vector<double> CSingleSystem::getSolidEffectiveLayerConductivities() const
    {
        std::vector<double> results;
        auto layers = getSolidLayers();
        for(auto & layer : layers)
        {
            results.emplace_back(layer->getEffectiveThermalConductivity());
        }
        return results;
    }

    std::vector<double> CSingleSystem::getGapEffectiveLayerConductivities() const
    {
        std::vector<double> results;
        auto gaps = getGapLayers();
        for(auto const & layer : gaps)
        {
            results.emplace_back(layer->getEffectiveThermalConductivity());
        }
        return results;
    }

    double CSingleSystem::EffectiveConductivity() const
    {
        auto temperatures = getTemperatures();
        const auto deltaTemp = std::abs(temperatures[0] - temperatures[temperatures.size() - 1]);
        return std::abs(thickness() * getHeatFlow(Environment::Indoor) / deltaTemp);
    }

    double CSingleSystem::thickness() const
    {
        double thickness{0};
        auto layers = getSolidLayers();
        for(auto & layer : layers)
        {
            thickness += layer->getThickness();
        }
        auto gaps = getGapLayers();
        for(auto const & gap : gaps)
        {
            thickness += gap->getThickness();
        }
        return thickness;
    }

    void CSingleSystem::setAbsorptances(const std::vector<double> & absorptances)
    {
        m_IGU.setAbsorptances(absorptances,
                              m_Environment.at(Environment::Outdoor)->getDirectSolarRadiation());
        solve();
    }

    void CSingleSystem::setWidth(double width)
    {
        m_IGU.setWidth(width);
    }

    void CSingleSystem::setHeight(double height)
    {
        m_IGU.setHeight(height);
    }

    void CSingleSystem::setTilt(const double tilt)
    {
        m_IGU.setTilt(tilt);
    }

    void CSingleSystem::setInteriorAndExteriorSurfacesHeight(double height)
    {
        std::ranges::for_each(m_Environment | std::views::values, [height](auto &environment) {
            environment->setHeight(height);
        });
    }

    void CSingleSystem::setDeflectionProperties(const double t_Tini, const double t_Pini)
    {
        m_IGU.setDeflectionProperties(t_Tini,
                                      t_Pini,
                                      m_Environment.at(Environment::Indoor)->getPressure(),
                                      m_Environment.at(Environment::Outdoor)->getPressure());

        // Need to throw previous solution off in case someone calculated CSingleSystem without
        // deflection and then turns deflection on, iterations will conclude that solution is
        // correct (Simon)
        initializeStartValues();
    }

    void CSingleSystem::setDeflectionProperties(const std::vector<double> & measuredGapWidths)
    {
        m_IGU.setDeflectionProperties(measuredGapWidths);
    }

    void CSingleSystem::clearDeflection()
    {
        m_IGU.clearDeflection();
    }

    void CSingleSystem::setSolidLayerConductivities(
      const std::vector<double> & t_SolidLayerThermalConductivities)
    {
        m_IGU.setSolidLayerConductivities(t_SolidLayerThermalConductivities);
    }

    void CSingleSystem::setSolidLayerConductivity(size_t t_LayerIndex,
                                                  double t_SolidLayerThermalConductivity)
    {
        m_IGU.setSolidLayerConductivity(t_LayerIndex, t_SolidLayerThermalConductivity);
    }

    std::vector<double> CSingleSystem::getGapPressures() const
    {
        return m_IGU.getGapPressures();
    }
}   // namespace Tarcog::ISO15099
