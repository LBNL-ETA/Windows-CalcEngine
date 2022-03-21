#pragma once

#include <memory>

#include "SpecularLayer.hpp"
#include "PhotovoltaicProperties.hpp"

namespace SingleLayerOptics
{
    class CMaterialPhotovoltaic;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // PhotovoltaicLayer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    class PhotovoltaicLayer : public SpecularLayer
    {
    public:
        static std::shared_ptr<PhotovoltaicLayer>
          createLayer(const std::shared_ptr<CMaterialPhotovoltaic> & material, PVPowerPropertiesTable powerTable);

        [[nodiscard]] FenestrationCommon::CSeries jscPrime(FenestrationCommon::Side t_Side) const;

        explicit PhotovoltaicLayer(const CSpecularCell & cell,
                                   const std::shared_ptr<CMaterialPhotovoltaic> & material);

        void assignPowerTable(PVPowerPropertiesTable powerTable);

        [[nodiscard]] double voc(double electricity) const;
        [[nodiscard]] double ff(double electricity) const;

    private:
        std::shared_ptr<CMaterialPhotovoltaic> m_PVMaterial;
        PVPowerPropertiesTable m_PVPowerTable;
    };

}   // namespace SingleLayerOptics