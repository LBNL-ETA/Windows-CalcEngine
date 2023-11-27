#pragma once

#include <memory>

#include "IGUSolidLayer.hpp"

namespace Tarcog::ISO15099
{
    ////////////////////////////////////////////////////////////////////////////
    ////    CIGUSolidLayerDeflection
    ////////////////////////////////////////////////////////////////////////////
    class CIGUSolidLayerDeflection : public CIGUSolidLayer
    {
    public:
        explicit CIGUSolidLayerDeflection(const CIGUSolidLayer & t_SolidLayer);
        CIGUSolidLayerDeflection(const CIGUSolidLayer & t_SolidLayer,
                                 double t_YoungsModulus,
                                 double t_PoisonRatio,
                                 double t_Density);

        double flexuralRigidity() const;

        std::shared_ptr<CBaseLayer> clone() const override;

        double youngsModulus() const override;
        double density() const override;

    private:
        double m_YoungsModulus;
        double m_PoisonRatio;
        double m_Density;
    };

    ////////////////////////////////////////////////////////////////////////////
    ////    CIGUDeflectionMeasuread
    ////////////////////////////////////////////////////////////////////////////
    class CIGUDeflectionMeasured : public CIGUSolidLayerDeflection
    {
    public:
        CIGUDeflectionMeasured(std::shared_ptr<CIGUSolidLayerDeflection> & t_Layer,
                                const double t_MeanDeflection,
                                const double t_MaxDeflection);
    };
}   // namespace Tarcog::ISO15099
