#include <WCECommon.hpp>

#include "PhotovoltaicProperties.hpp"

namespace SingleLayerOptics
{
    ///////////////////////////////////////////////////////////////////////////
    /// PVPowerProperties
    ///////////////////////////////////////////////////////////////////////////
    PVPowerProperties::PVPowerProperties(double jsc, double voc, double ff) :
        JSC(jsc), VOC(voc), FF(ff)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// PVPowerPropertiesTable
    ///////////////////////////////////////////////////////////////////////////
    PVPowerPropertiesTable::PVPowerPropertiesTable(
      std::vector<PVPowerProperties> pvPowerProperties) :
        m_PVPowerProperties(std::move(pvPowerProperties))
    {}

    double PVPowerPropertiesTable::voc(const double jsc) const
    {
        const auto el{jsc / 10};   // Need to convert from standard SI into what user provided
        const auto ind{findIndexes(el)};
        const auto value{
          FenestrationCommon::linearInterpolation(m_PVPowerProperties[ind.first].JSC,
                                                  m_PVPowerProperties[ind.second].JSC,
                                                  m_PVPowerProperties[ind.first].VOC,
                                                  m_PVPowerProperties[ind.second].VOC,
                                                  el)};

        return value;
    }

    double PVPowerPropertiesTable::ff(double jsc) const
    {
        const auto el{jsc / 10};   // Need to convert from standard SI into what user provided
        const auto ind{findIndexes(el)};
        const auto value{
          FenestrationCommon::linearInterpolation(m_PVPowerProperties[ind.first].JSC,
                                                  m_PVPowerProperties[ind.second].JSC,
                                                  m_PVPowerProperties[ind.first].FF,
                                                  m_PVPowerProperties[ind.second].FF,
                                                  el)};

        return value;
    }

    PVPowerPropertiesTable::SearchIndexes PVPowerPropertiesTable::findIndexes(const double el) const
    {
        size_t index{0u};
        for(size_t i = 0u; i < m_PVPowerProperties.size(); ++i)
        {
            if(el > m_PVPowerProperties[i].JSC)
            {
                index = i;
            }
        }

        const auto lastIndex{index == m_PVPowerProperties.size() ? index : index + 1u};
        return {index, lastIndex};
    }
}   // namespace SingleLayerOptics
