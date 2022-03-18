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

    std::vector<FenestrationCommon::CSeries>
      PhotovoltaicSpecularBSDFLayer::jscPrime(FenestrationCommon::Side t_Side, const std::vector<double> & wavelengths) const
    {
        std::vector<FenestrationCommon::CSeries> result;
        auto jscPrime{m_PVMaterial->jscPrime(t_Side)};
        if(jscPrime.size() != wavelengths.size())
        {
            jscPrime = jscPrime.interpolate(wavelengths);
        }
        for(const auto & jsc : jscPrime)
        {
            FenestrationCommon::CSeries curVal;

            // No angular dependence for jsc for now. Every direction will have the same electricity generation.
            for(size_t i = 0u; i < m_BSDFHemisphere.getDirections(BSDFDirection::Incoming).size();
                ++i)
            {
                curVal.addProperty(jsc->x(), jsc->value());
            }
            result.push_back(curVal);
        }
        return result;
    }

    void PhotovoltaicSpecularBSDFLayer::assignPowerTable(PVPowerPropertiesTable powerTable)
    {
        m_PVPowerTable = std::move(powerTable);
    }

    double PhotovoltaicSpecularBSDFLayer::voc(double electricity) const
    {
        return m_PVPowerTable.voc(electricity);
    }

    double PhotovoltaicSpecularBSDFLayer::ff(double electricity) const
    {
        return m_PVPowerTable.ff(electricity);
    }
}   // namespace SingleLayerOptics