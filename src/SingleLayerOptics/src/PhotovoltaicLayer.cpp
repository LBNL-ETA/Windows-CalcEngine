#include "PhotovoltaicLayer.hpp"

#include <utility>
#include "MaterialDescription.hpp"

namespace SingleLayerOptics
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // PhotovoltaicLayer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    PhotovoltaicLayer::PhotovoltaicLayer(const CSpecularCell & cell,
                                         const std::shared_ptr<CMaterialPhotovoltaic> & material) :
        SpecularLayer(cell), m_PVMaterial(material)
    {}

    void PhotovoltaicLayer::assignPowerTable(PVPowerPropertiesTable powerTable)
    {
        m_PVPowerTable = std::move(powerTable);
    }

    double PhotovoltaicLayer::voc(double electricity) const
    {
        return m_PVPowerTable.voc(electricity);
    }

    double PhotovoltaicLayer::ff(double electricity) const
    {
        return m_PVPowerTable.ff(electricity);
    }

    std::shared_ptr<PhotovoltaicLayer>
      PhotovoltaicLayer::createLayer(const std::shared_ptr<CMaterialPhotovoltaic> & material, PVPowerPropertiesTable powerTable)
    {
        auto aCell = CSpecularCell(material);
        auto layer{std::make_shared<PhotovoltaicLayer>(aCell, material)};

        layer->assignPowerTable(std::move(powerTable));

        return layer;
    }

    FenestrationCommon::CSeries PhotovoltaicLayer::jscPrime(FenestrationCommon::Side t_Side) const
    {
        return m_PVMaterial->jscPrime(t_Side);
    }
}   // namespace SingleLayerOptics