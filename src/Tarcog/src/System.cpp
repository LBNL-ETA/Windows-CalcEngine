#include "System.hpp"
#include "IGU.hpp"
#include "Environment.hpp"
#include "IGUSolidLayer.hpp"
#include "SingleSystem.hpp"


namespace Tarcog::ISO15099
{
    CSystem::CSystem(CIGU & t_IGU,
                     std::shared_ptr<CEnvironment> const & t_Indoor,
                     std::shared_ptr<CEnvironment> const & t_Outdoor)
    {
        m_System[System::SHGC] = std::make_shared<CSingleSystem>(t_IGU, t_Indoor, t_Outdoor);
        m_System[System::Uvalue] = std::make_shared<CSingleSystem>(
          t_IGU, t_Indoor->cloneEnvironment(), t_Outdoor->cloneEnvironment());
        m_System.at(System::Uvalue)->setSolarRadiation(0);

        solve();
    }

    std::vector<double> CSystem::getTemperatures(System const t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getTemperatures();
    }

    std::vector<double> CSystem::getRadiosities(System const t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getRadiosities();
    }

    std::vector<double> CSystem::getMaxLayerDeflections(System t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getMaxLayerDeflections();
    }

    std::vector<double> CSystem::getMeanLayerDeflections(System t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getMeanLayerDeflections();
    }

    std::vector<double> CSystem::getMaxGapWidth(System t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getMaxGapWidth();
    }

    std::vector<double> CSystem::getMeanGapWidth(System t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getMeanGapWidth();
    }

    std::vector<double> CSystem::getPanesLoad(System t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getPanesLoad();
    }

    std::vector<double> CSystem::getGapPressures(System t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getGapPressures();
    }

    void CSystem::setAppliedLoad(const std::vector<double> & load)
    {
        m_Solved = false;
        for(auto & [key, value] : m_System)
        {
            std::ignore = key;
            value->setAppliedLoad(load);
        }
    }

    std::vector<std::shared_ptr<CIGUSolidLayer>>
      CSystem::getSolidLayers(System const t_System) const
    {
        return m_System.at(t_System)->getSolidLayers();
    }

    std::vector<std::shared_ptr<CIGUGapLayer>> CSystem::getGapLayers(System t_System) const
    {
        return m_System.at(t_System)->getGapLayers();
    }

    double CSystem::getHeatFlow(System const t_System, Environment const t_Environment)
    {
        checkSolved();
        return m_System.at(t_System)->getHeatFlow(t_Environment);
    }

    double CSystem::getUValue()
    {
        checkSolved();
        return m_System.at(System::Uvalue)->getUValue();
    }

    double CSystem::getSHGC(double const t_TotSol)
    {
        checkSolved();
        const auto ventilatedFlowSHGC{
          m_System.at(System::SHGC)->getVentilationFlow(Environment::Indoor)};
        const auto ventilatedFlowU{
          m_System.at(System::Uvalue)->getVentilationFlow(Environment::Indoor)};

        const auto indoorFlowSHGC{m_System.at(System::SHGC)->getHeatFlow(Environment::Indoor)
                                  + ventilatedFlowSHGC};
        const auto indoorFlowU{m_System.at(System::Uvalue)->getHeatFlow(Environment::Indoor)
                               + ventilatedFlowU};

        auto result{m_System.at(System::SHGC)->getSolarRadiation() != 0.0
                      ? t_TotSol
                          - (indoorFlowSHGC - indoorFlowU)
                              / m_System.at(System::SHGC)->getSolarRadiation()
                      : 0};

        return result;
    }

    size_t CSystem::getNumberOfIterations(System const t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getNumberOfIterations();
    }

    std::vector<double> CSystem::getSolidEffectiveLayerConductivities(const System t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getSolidEffectiveLayerConductivities();
    }

    std::vector<double> CSystem::getGapEffectiveLayerConductivities(const System t_System)
    {
        checkSolved();
        return m_System.at(t_System)->getGapEffectiveLayerConductivities();
    }

    double CSystem::getEffectiveSystemConductivity(const System t_System)
    {
        checkSolved();
        return m_System.at(t_System)->EffectiveConductivity();
    }

    double CSystem::thickness(const System t_System) const
    {
        return m_System.at(t_System)->thickness();
    }

    double CSystem::thickness() const
    {
        return thickness(System::Uvalue);
    }

    namespace Helper
    {
        template<typename LayerGetter>
        std::vector<double> getLayerThicknesses(const CSystem * system, LayerGetter getter)
        {
            auto layers = (system->*getter)(System::Uvalue);
            std::vector<double> result(layers.size());
            std::ranges::transform(
              layers, result.begin(), [](const auto & layer) { return layer->getThickness(); });
            return result;
        }
    }   // namespace Helper

    std::vector<double> CSystem::solidLayerThicknesses() const
    {
        return Helper::getLayerThicknesses(this, &CSystem::getSolidLayers);
    }

    std::vector<double> CSystem::gapLayerThicknesses() const
    {
        return Helper::getLayerThicknesses(this, &CSystem::getGapLayers);
    }

    double CSystem::relativeHeatGain(const double Tsol)
    {
        return getUValue() * 7.78 + getSHGC(Tsol) / 0.87 * 630.9;
    }

    void CSystem::setAbsorptances(const std::vector<double> & absorptances)
    {
        m_System.at(System::SHGC)->setAbsorptances(absorptances);
        m_Solved = false;
    }

    void CSystem::setWidth(double width)
    {
        for(auto & system : m_System)
        {
            system.second->setWidth(width);
        }
        m_Solved = false;
    }

    void CSystem::setHeight(double height)
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->setHeight(height);
        }
        m_Solved = false;
    }

    void CSystem::setTilt(double tilt)
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->setTilt(tilt);
        }
        m_Solved = false;
    }

    void CSystem::setWidthAndHeight(double width, double height)
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->setWidth(width);
            system->setHeight(height);
        }
        m_Solved = false;
    }

    void CSystem::setInteriorAndExteriorSurfacesHeight(double height)
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->setInteriorAndExteriorSurfacesHeight(height);
        }
        m_Solved = false;
    }

    void CSystem::solve()
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->solve();
        }
        m_Solved = true;
    }

    void CSystem::checkSolved()
    {
        if(!m_Solved)
        {
            solve();
            m_Solved = true;
        }
    }

    double CSystem::getH(System sys, Environment environment) const
    {
        return m_System.at(sys)->getH(environment);
    }

    void CSystem::setDeflectionProperties(double t_Tini, double t_Pini)
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->setDeflectionProperties(t_Tini, t_Pini);
        }
        m_Solved = false;
    }

    void CSystem::setDeflectionProperties(const std::vector<double> & measuredGapWidths)
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->setDeflectionProperties(measuredGapWidths);
        }
        m_Solved = false;
    }

    void CSystem::clearDeflection()
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->clearDeflection();
        }
        m_Solved = false;
    }

    void CSystem::setSolidLayerConductivities(
      const std::vector<double> & t_SolidLayerThermalConductivities)
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->setSolidLayerConductivities(t_SolidLayerThermalConductivities);
        }
        m_Solved = false;
    }

    void CSystem::setSolidLayerConductivity(size_t t_LayerIndex,
                                            double t_SolidLayerThermalConductivity)
    {
        for(auto & [key, system] : m_System)
        {
            std::ignore = key;
            system->setSolidLayerConductivity(t_LayerIndex, t_SolidLayerThermalConductivity);
        }
        m_Solved = false;
    }

}   // namespace Tarcog::ISO15099
