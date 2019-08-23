#pragma once

#include <memory>

#include "SpecularLayer.hpp"

namespace SingleLayerOptics {

    class CMaterialPhotovoltaic;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // PhotovoltaicLayer
    ////////////////////////////////////////////////////////////////////////////////////////////////

    class PhotovoltaicLayer : public SpecularLayer
    {
    public:
        static PhotovoltaicLayer createLayer(const std::shared_ptr<CMaterialPhotovoltaic> & material);

        FenestrationCommon::CSeries PCE(FenestrationCommon::Side t_Side) const;
        FenestrationCommon::CSeries W(FenestrationCommon::Side t_Side) const;

    private:
        explicit PhotovoltaicLayer(const CSpecularCell & cell, const std::shared_ptr<CMaterialPhotovoltaic> & material);
        std::shared_ptr<CMaterialPhotovoltaic> m_PVMaterial;
    };

}