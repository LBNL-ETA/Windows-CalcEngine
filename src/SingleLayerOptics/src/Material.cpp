#include "Material.hpp"

#include "MaterialDescription.hpp"
#include "WCECommon.hpp"

using FenestrationCommon::WavelengthRange;

namespace SingleLayerOptics
{
    std::shared_ptr<CMaterial> Material::dualBandMaterial(const double Tfsol,
                                                          const double Tbsol,
                                                          const double Rfsol,
                                                          const double Rbsol,
                                                          const double Tfvis,
                                                          const double Tbvis,
                                                          const double Rfvis,
                                                          const double Rbvis)
    {
        auto aSolarRangeMaterial =
          std::make_shared<CMaterialSingleBand>(Tfsol, Tbsol, Rfsol, Rbsol, WavelengthRange::Solar);
        auto aVisibleRangeMaterial = std::make_shared<CMaterialSingleBand>(
          Tfvis, Tbvis, Rfvis, Rbvis, WavelengthRange::Visible);
        return std::make_shared< CMaterialDualBand >( aVisibleRangeMaterial, aSolarRangeMaterial );
    }
}   // namespace SingleLayerOptics