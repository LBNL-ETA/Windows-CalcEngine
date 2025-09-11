#pragma once

#include <memory>

#include "SpecularLayer.hpp"
#include "PhotovoltaicProperties.hpp"

namespace SingleLayerOptics
{
    class CMaterialPhotovoltaicSample;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // PhotovoltaicSpecularLayer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    class PhotovoltaicSpecularLayer : public SpecularLayer
    {
    public:
        static std::shared_ptr<PhotovoltaicSpecularLayer>
          createLayer(const std::shared_ptr<CMaterial> & material,
                      PVPowerPropertiesTable powerTable);

        [[nodiscard]] FenestrationCommon::CSeries jscPrime(FenestrationCommon::Side t_Side) const;

        explicit PhotovoltaicSpecularLayer(const CSpecularCell & cell,
                                           const std::shared_ptr<CMaterial> & material);

        void assignPowerTable(PVPowerPropertiesTable powerTable);

        [[nodiscard]] double voc(double electricity) const;
        [[nodiscard]] double ff(double electricity) const;

    private:
        std::shared_ptr<CMaterial> m_PVMaterial;
        PVPowerPropertiesTable m_PVPowerTable;
    };

}   // namespace SingleLayerOptics