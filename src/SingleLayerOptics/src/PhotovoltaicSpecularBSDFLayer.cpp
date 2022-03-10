#include "PhotovoltaicSpecularBSDFLayer.hpp"

namespace SingleLayerOptics
{
    PhotovoltaicSpecularBSDFLayer::PhotovoltaicSpecularBSDFLayer(
      const std::shared_ptr<CSpecularCell> & t_Cell, const CBSDFHemisphere & t_Hemisphere) :
        CSpecularBSDFLayer(t_Cell, t_Hemisphere)
    {}

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