#pragma once

#include <memory>

#include "PhotovoltaicProperties.hpp"
#include "SpecularBSDFLayer.hpp"

namespace SingleLayerOptics
{
    class CSpecularCell;
    class CMaterialPhotovoltaic;

    class PhotovoltaicSpecularBSDFLayer : public CSpecularBSDFLayer
    {
    public:
        PhotovoltaicSpecularBSDFLayer(const std::shared_ptr<CSpecularCell> & t_Cell,
                                      const CBSDFHemisphere & t_Hemisphere);

        void assignPowerTable(PVPowerPropertiesTable powerTable);

        [[nodiscard]] double voc(double electricity) const;
        [[nodiscard]] double ff(double electricity) const;

    private:
        std::shared_ptr<CMaterialPhotovoltaic> m_PVMaterial;
        PVPowerPropertiesTable m_PVPowerTable;
    };
}   // namespace SingleLayerOptics