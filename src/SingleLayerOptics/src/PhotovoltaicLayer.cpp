#include "PhotovoltaicLayer.hpp"

#include <utility>
#include "MaterialDescription.hpp"

namespace SingleLayerOptics
{
    ///////////////////////////////////////////////////////////////////////////
    /// PVPowerProperties
    ///////////////////////////////////////////////////////////////////////////
    [[maybe_unused]] PVPowerProperties::PVPowerProperties(double jsc, double voc, double ff) :
        JSC(jsc), VOC(voc), FF(ff)
    {}

    ///////////////////////////////////////////////////////////////////////////
    /// PVPowerPropertiesTable
    ///////////////////////////////////////////////////////////////////////////
    PVPowerPropertiesTable::PVPowerPropertiesTable(
      std::vector<PVPowerProperties> pvPowerProperties) : m_PVPowerProperties(
        std::move(pvPowerProperties))
    {}

    double PVPowerPropertiesTable::voc(double jsc) const
    {
        // TODO: Calculate value from the table
        return 0;
    }

    double PVPowerPropertiesTable::ff(double jsc) const
    {
        // TODO: Calculate value from the table
        return 0;
    }

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

    std::shared_ptr<PhotovoltaicLayer>
      PhotovoltaicLayer::createLayer(const std::shared_ptr<CMaterialPhotovoltaic> & material)
    {
        auto aCell = CSpecularCell(material);
        return std::make_shared<PhotovoltaicLayer>(aCell, material);
    }

    FenestrationCommon::CSeries PhotovoltaicLayer::jscPrime(FenestrationCommon::Side t_Side) const
    {
        return m_PVMaterial->jscPrime(t_Side);
    }
}   // namespace SingleLayerOptics