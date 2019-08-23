#include "PhotovoltaicLayer.hpp"
#include "MaterialDescription.hpp"

namespace SingleLayerOptics
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // PhotovoltaicLayer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    PhotovoltaicLayer::PhotovoltaicLayer(const CSpecularCell & cell,
                                         const std::shared_ptr<CMaterialPhotovoltaic> & material) :
        SpecularLayer(cell),
        m_PVMaterial(material)
    {}

    PhotovoltaicLayer
      PhotovoltaicLayer::createLayer(const std::shared_ptr<CMaterialPhotovoltaic> & material)
    {
        auto aCell = CSpecularCell(material);
        return PhotovoltaicLayer(aCell, material);
    }

    FenestrationCommon::CSeries PhotovoltaicLayer::PCE(FenestrationCommon::Side t_Side) const
    {
        return m_PVMaterial->PCE(t_Side);
    }

    FenestrationCommon::CSeries PhotovoltaicLayer::W(FenestrationCommon::Side t_Side) const
    {
        return m_PVMaterial->W(t_Side);
    }
}   // namespace SingleLayerOptics