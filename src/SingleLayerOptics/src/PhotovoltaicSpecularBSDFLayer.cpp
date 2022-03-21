#include "PhotovoltaicSpecularBSDFLayer.hpp"
#include "MaterialDescription.hpp"
#include "SpecularCell.hpp"

namespace SingleLayerOptics
{
    PhotovoltaicSpecularBSDFLayer::PhotovoltaicSpecularBSDFLayer(
      const std::shared_ptr<CSpecularCell> & t_Cell, const CBSDFHemisphere & t_Hemisphere) :
        CSpecularBSDFLayer(t_Cell, t_Hemisphere)
    {
        const auto material(t_Cell->getMaterial());
        if(std::dynamic_pointer_cast<CMaterialPhotovoltaic>(material))
        {
            m_PVMaterial = std::dynamic_pointer_cast<CMaterialPhotovoltaic>(material);
        }
        else
        {
            throw std::runtime_error("Incorrect material for photovoltaic layer.");
        }
    }

    std::vector<std::vector<double>>
      PhotovoltaicSpecularBSDFLayer::jscPrime(FenestrationCommon::Side t_Side, const std::vector<double> & wavelengths) const
    {
        std::vector<std::vector<double>> result;
        auto jscPrime{m_PVMaterial->jscPrime(t_Side)};
        if(jscPrime.size() != wavelengths.size())
        {
            jscPrime = jscPrime.interpolate(wavelengths);
        }
        for(const auto & jsc : jscPrime)
        {
            std::vector<double> curVal;

            // No angular dependence for jsc for now. Every direction will have the same electricity generation.
            for(size_t i = 0u; i < m_BSDFHemisphere.getDirections(BSDFDirection::Incoming).size();
                ++i)
            {
                curVal.push_back(jsc->value());
            }
            result.push_back(curVal);
        }
        return result;
    }

    void PhotovoltaicSpecularBSDFLayer::assignPowerTable(PVPowerPropertiesTable powerTable)
    {
        m_PVPowerTable = std::move(powerTable);
    }

    std::vector<double>
      PhotovoltaicSpecularBSDFLayer::voc(const std::vector<double> & electricalCurrent) const
    {
        std::vector<double> result;
        result.reserve(electricalCurrent.size());
        for(const auto & cur: electricalCurrent)
        {
            result.push_back(m_PVPowerTable.voc(cur));
        }
        return result;
    }

    std::vector<double>
      PhotovoltaicSpecularBSDFLayer::ff(const std::vector<double> & electricalCurrent) const
    {
        std::vector<double> result;
        result.reserve(electricalCurrent.size());
        for(const auto & cur : electricalCurrent)
        {
            result.push_back(m_PVPowerTable.ff(cur));
        }
        return result;
    }
}   // namespace SingleLayerOptics