#include "System.hpp"
#include "IGU.hpp"
#include "Environment.hpp"
#include "SingleSystem.hpp"


namespace Tarcog
{
    namespace ISO15099
    {
        CSystem::CSystem(CIGU & t_IGU,
                         std::shared_ptr<CEnvironment> const & t_Indoor,
                         std::shared_ptr<CEnvironment> const & t_Outdoor)
        {
            m_System[System::SHGC] = std::make_shared<CSingleSystem>(t_IGU, t_Indoor, t_Outdoor);
            m_System[System::Uvalue] = std::make_shared<CSingleSystem>(*m_System.at(System::SHGC));
            m_System.at(System::Uvalue)->setSolarRadiation(0);

            for(auto & aSystem : m_System)
            {
                aSystem.second->solve();
            }
        }

        std::vector<double> CSystem::getTemperatures(System const t_System) const
        {
            return m_System.at(t_System)->getTemperatures();
        }

        std::vector<double> CSystem::getRadiosities(System const t_System) const
        {
            return m_System.at(t_System)->getRadiosities();
        }

        std::vector<double> CSystem::getMaxDeflections(System const t_System) const
        {
            return m_System.at(t_System)->getMaxDeflections();
        }

        std::vector<double> CSystem::getMeanDeflections(System const t_System) const
        {
            return m_System.at(t_System)->getMeanDeflections();
        }

        std::vector<std::shared_ptr<CIGUSolidLayer>>
          CSystem::getSolidLayers(System const t_System) const
        {
            return m_System.at(t_System)->getSolidLayers();
        }

        double CSystem::getHeatFlow(System const t_System, Environment const t_Environment) const
        {
            return m_System.at(t_System)->getHeatFlow(t_Environment);
        }

        double CSystem::getUValue() const
        {
            return m_System.at(System::Uvalue)->getUValue();
        }

        double CSystem::getSHGC(double const t_TotSol) const
        {
            return t_TotSol
                   - (m_System.at(System::SHGC)->getHeatFlow(Environment::Indoor)
                      - m_System.at(System::Uvalue)->getHeatFlow(Environment::Indoor))
                       / m_System.at(System::SHGC)->getSolarRadiation();
        }

        size_t CSystem::getNumberOfIterations(System const t_System) const
        {
            return m_System.at(t_System)->getNumberOfIterations();
        }

        std::vector<double>
          CSystem::getSolidEffectiveLayerConductivities(const System t_System) const
        {
            return m_System.at(t_System)->getSolidEffectiveLayerConductivities();
        }

        std::vector<double> CSystem::getGapEffectiveLayerConductivities(const System t_System) const
        {
            return m_System.at(t_System)->getGapEffectiveLayerConductivities();
        }

        double CSystem::getEffectiveSystemConductivity(const System t_System) const
        {
            return m_System.at(t_System)->EffectiveConductivity();
        }

        double CSystem::thickness(const System t_System) const
        {
            return m_System.at(t_System)->thickness();
        }

        double CSystem::relativeHeatGain(const double Tsol) const
        {
            return getUValue() * 7.78 + getSHGC(Tsol) / 0.87 * 630.9;
        }

        void CSystem::setAbsorptances(const std::vector<double> & absorptances)
        {
            m_System.at(System::SHGC)->setAbsorptances(absorptances);
        }

    }   // namespace ISO15099

}   // namespace Tarcog
