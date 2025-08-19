#include <cmath>

#include "IGUSolidDeflection.hpp"
#include "Surface.hpp"
#include "TarcogConstants.hpp"

using FenestrationCommon::Side;

namespace Tarcog::ISO15099
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///    CIGUSolidLayerDeflection
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CIGUSolidLayerDeflection::CIGUSolidLayerDeflection(CIGUSolidLayer const & t_SolidLayer) :
        CIGUSolidLayer(t_SolidLayer),
        m_YoungsModulus(DeflectionConstants::YOUNGSMODULUS),
        m_PoisonRatio(DeflectionConstants::POISONRATIO),
        m_Density(MaterialConstants::GLASSDENSITY)
    {}

    CIGUSolidLayerDeflection::CIGUSolidLayerDeflection(CIGUSolidLayer const & t_SolidLayer,
                                                       double const t_YoungsModulus,
                                                       double const t_PoisonRatio,
                                                       double const t_Density) :
        CIGUSolidLayer(t_SolidLayer),
        m_YoungsModulus(t_YoungsModulus),
        m_PoisonRatio(t_PoisonRatio),
        m_Density(t_Density)
    {}

    double CIGUSolidLayerDeflection::flexuralRigidity() const
    {
        return m_YoungsModulus * pow(m_Thickness, 3) / (12 * (1 - pow(m_PoisonRatio, 2)));
    }

    std::shared_ptr<CBaseLayer> CIGUSolidLayerDeflection::clone() const
    {
        return std::make_shared<CIGUSolidLayerDeflection>(*this);
    }

    double CIGUSolidLayerDeflection::youngsModulus() const
    {
        return m_YoungsModulus;
    }

    double CIGUSolidLayerDeflection::density() const
    {
        return m_Density;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///    CIGUDeflectionMeasured
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CIGUDeflectionMeasured::CIGUDeflectionMeasured(
      std::shared_ptr<CIGUSolidLayerDeflection> & t_Layer,
      const double t_MeanDeflection,
      const double t_MaxDeflection) :
        CIGUSolidLayerDeflection(*t_Layer)
    {
        for(Side aSide : FenestrationCommon::sides())
        {
            m_Surface[aSide]->applyDeflection(t_MeanDeflection, t_MaxDeflection);
        }
    }

}   // namespace Tarcog::ISO15099
