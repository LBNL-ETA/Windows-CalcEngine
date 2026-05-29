#include "PhotovoltaicSpecularLayer.hpp"

#include <utility>
#include "MaterialDescription.hpp"

namespace SingleLayerOptics
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // PhotovoltaicSpecularLayer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    PhotovoltaicSpecularLayer::PhotovoltaicSpecularLayer(
      CBaseCell cell, const std::shared_ptr<CMaterial> & material) :
        SpecularLayer(std::move(cell)), m_PVMaterial(material)
    {}

    void PhotovoltaicSpecularLayer::assignPowerTable(PVPowerPropertiesTable powerTable)
    {
        m_PVPowerTable = std::move(powerTable);
    }

    double PhotovoltaicSpecularLayer::voc(double electricity) const
    {
        return m_PVPowerTable.voc(electricity);
    }

    double PhotovoltaicSpecularLayer::ff(double electricity) const
    {
        return m_PVPowerTable.ff(electricity);
    }

    std::shared_ptr<PhotovoltaicSpecularLayer>
      PhotovoltaicSpecularLayer::createLayer(const std::shared_ptr<CMaterial> & material,
                                             PVPowerPropertiesTable powerTable)
    {
        auto layer{
          std::make_shared<PhotovoltaicSpecularLayer>(makeSpecularCell(material), material)};
        layer->assignPowerTable(std::move(powerTable));
        return layer;
    }

    FenestrationCommon::CSeries
      PhotovoltaicSpecularLayer::jscPrime(FenestrationCommon::Side t_Side) const
    {
        return m_PVMaterial->jscPrime(t_Side);
    }
}   // namespace SingleLayerOptics
