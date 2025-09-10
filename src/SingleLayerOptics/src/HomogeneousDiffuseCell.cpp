#include "HomogeneousDiffuseCell.hpp"
#include "MaterialDescription.hpp"

// Important note for homogeneous diffuse cell is that no angular calculations should be performed
// here which is why every function is creating CBeamDirection() that is defaulted to the normal
// incidence. Additional parameter at the end is to tell material that direct hemispherical value is
// required from it.

namespace SingleLayerOptics
{
    CHomogeneousDiffuseCell::CHomogeneousDiffuseCell(
      const std::shared_ptr<CMaterial> & t_MaterialProperties,
      const std::shared_ptr<ICellDescription> & t_Cell,
      double rotation) :
        CBaseCell(t_MaterialProperties, t_Cell, rotation),
        CDirectionalDiffuseCell(t_MaterialProperties, t_Cell, rotation)
    {}

    double CHomogeneousDiffuseCell::T_dir_dif(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection &,
                                              const CBeamDirection &)
    {
        const double cellT = CBaseCell::T_dir_dir(t_Side, CBeamDirection());
        const double materialT = m_Material->getProperty(FenestrationCommon::Property::T,
                                                         t_Side,
                                                         CBeamDirection(),
                                                         CBeamDirection(),
                                                         OutgoingAggregation::Hemispherical);
        return cellT + (1 - cellT) * materialT;
    }

    double CHomogeneousDiffuseCell::R_dir_dif(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection &,
                                              const CBeamDirection &)
    {
        const double cellT = CBaseCell::T_dir_dir(t_Side, CBeamDirection());
        const double cellR = CBaseCell::R_dir_dir(t_Side, CBeamDirection());
        const double materialR = m_Material->getProperty(FenestrationCommon::Property::R,
                                                         t_Side,
                                                         CBeamDirection(),
                                                         CBeamDirection(),
                                                         OutgoingAggregation::Hemispherical);
        return cellR + (1 - cellT) * materialR;
    }

    std::vector<double> CHomogeneousDiffuseCell::T_dir_dif_band(
      const FenestrationCommon::Side t_Side, const CBeamDirection &, const CBeamDirection &)
    {
        double cellT = CBaseCell::T_dir_dir(t_Side, CBeamDirection());

        auto materialBandValues = m_Material->getBandProperties(FenestrationCommon::Property::T,
                                                                t_Side,
                                                                CBeamDirection(),
                                                                CBeamDirection(),
                                                                OutgoingAggregation::Hemispherical);

        std::vector<double> result;
        result.reserve(materialBandValues.size());
        for(auto materialBandValue : materialBandValues)
        {
            result.push_back(cellT + (1 - cellT) * materialBandValue);
        }

        return result;
    }

    double CHomogeneousDiffuseCell::T_dir_dif_by_wavelength(const FenestrationCommon::Side t_Side,
                                                            const CBeamDirection &,
                                                            const CBeamDirection &,
                                                            size_t wavelengthIndex)
    {
        double cellT = CBaseCell::T_dir_dir(t_Side, CBeamDirection());
        auto materialTransmittance =
          m_Material->getBandProperty(FenestrationCommon::Property::T,
                                      t_Side,
                                      wavelengthIndex,
                                      CBeamDirection(),
                                      CBeamDirection(),
                                      OutgoingAggregation::Hemispherical);
        return cellT + (1 - cellT) * materialTransmittance;
    }

    std::vector<double> CHomogeneousDiffuseCell::R_dir_dif_band(
      const FenestrationCommon::Side t_Side, const CBeamDirection &, const CBeamDirection &)
    {
        double cellT = CBaseCell::T_dir_dir(t_Side, CBeamDirection());
        double cellR = CBaseCell::R_dir_dir(t_Side, CBeamDirection());

        auto materialBandValues = m_Material->getBandProperties(FenestrationCommon::Property::R,
                                                                t_Side,
                                                                CBeamDirection(),
                                                                CBeamDirection(),
                                                                OutgoingAggregation::Hemispherical);

        std::vector<double> result;
        result.reserve(materialBandValues.size());
        for(auto materialBandValue : materialBandValues)
        {
            result.push_back(cellR + (1 - cellT) * materialBandValue);
        }

        return result;
    }
    double CHomogeneousDiffuseCell::R_dir_dif_by_wavelength(const FenestrationCommon::Side t_Side,
                                                            const CBeamDirection &,
                                                            const CBeamDirection &,
                                                            size_t wavelengthIndex)
    {
        double cellT = CBaseCell::T_dir_dir(t_Side, CBeamDirection());
        double cellR = CBaseCell::R_dir_dir(t_Side, CBeamDirection());
        auto materialValue = m_Material->getBandProperty(FenestrationCommon::Property::R,
                                                         t_Side,
                                                         wavelengthIndex,
                                                         CBeamDirection(),
                                                         CBeamDirection(),
                                                         OutgoingAggregation::Hemispherical);
        return cellR + (1 - cellT) * materialValue;
    }

}   // namespace SingleLayerOptics
