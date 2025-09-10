#include "DirectionalDiffuseCell.hpp"
#include "MaterialDescription.hpp"

namespace SingleLayerOptics
{
    CDirectionalDiffuseCell::CDirectionalDiffuseCell(
      const std::shared_ptr<CMaterial> & t_MaterialProperties,
      const std::shared_ptr<ICellDescription> & t_Cell,
      double rotation) :
        CBaseCell(t_MaterialProperties, t_Cell, rotation)
    {}

    double CDirectionalDiffuseCell::T_dir_dif(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection & t_IncomingDirection,
                                              const CBeamDirection & t_OutgoingDirection)
    {
        const double cellT = CBaseCell::T_dir_dir(t_Side, t_IncomingDirection);
        const double materialT = m_Material->getProperty(
          FenestrationCommon::Property::T, t_Side, t_IncomingDirection, t_OutgoingDirection);
        return cellT + (1 - cellT) * materialT;
    }

    double CDirectionalDiffuseCell::R_dir_dif(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection & t_IncomingDirection,
                                              const CBeamDirection & t_OutgoingDirection)
    {
        const double cellT = CBaseCell::T_dir_dir(t_Side, t_IncomingDirection);
        const double cellR = CBaseCell::R_dir_dir(t_Side, t_IncomingDirection);
        const double materialR = m_Material->getProperty(
          FenestrationCommon::Property::R, t_Side, t_IncomingDirection, t_OutgoingDirection);
        return cellR + (1 - cellT) * materialR;
    }

    std::vector<double>
      CDirectionalDiffuseCell::T_dir_dif_band(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection & t_IncomingDirection,
                                              const CBeamDirection & t_OutgoingDirection)
    {
        double cellT = CBaseCell::T_dir_dir(t_Side, t_IncomingDirection);

        auto materialBandValues = m_Material->getBandProperties(
          FenestrationCommon::Property::T, t_Side, t_IncomingDirection, t_OutgoingDirection);

        std::vector<double> result;
        result.reserve(materialBandValues.size());
        for(auto materialBandValue : materialBandValues)
        {
            result.push_back(cellT + (1 - cellT) * materialBandValue);
        }

        return result;
    }

    double
      CDirectionalDiffuseCell::T_dir_dif_by_wavelength(const FenestrationCommon::Side t_Side,
                                                       const CBeamDirection & t_IncomingDirection,
                                                       const CBeamDirection & t_OutgoingDirection,
                                                       size_t wavelengthIndex)
    {
        double cellT = CBaseCell::T_dir_dir(t_Side, t_IncomingDirection);
        auto materialTransmittance = m_Material->getBandProperty(FenestrationCommon::Property::T,
                                                                 t_Side,
                                                                 wavelengthIndex,
                                                                 t_IncomingDirection,
                                                                 t_OutgoingDirection);
        return cellT + (1 - cellT) * materialTransmittance;
    }

    std::vector<double>
      CDirectionalDiffuseCell::R_dir_dif_band(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection & t_IncomingDirection,
                                              const CBeamDirection & t_OutgoingDirection)
    {
        double cellT = CBaseCell::T_dir_dir(t_Side, t_IncomingDirection);
        double cellR = CBaseCell::R_dir_dir(t_Side, t_IncomingDirection);

        auto materialBandValues = m_Material->getBandProperties(
          FenestrationCommon::Property::R, t_Side, t_IncomingDirection, t_OutgoingDirection);

        std::vector<double> result;
        result.reserve(materialBandValues.size());
        for(auto materialBandValue : materialBandValues)
        {
            result.push_back(cellR + (1 - cellT) * materialBandValue);
        }

        return result;
    }
    double
      CDirectionalDiffuseCell::R_dir_dif_by_wavelength(const FenestrationCommon::Side t_Side,
                                                       const CBeamDirection & t_IncomingDirection,
                                                       const CBeamDirection & t_OutgoingDirection,
                                                       size_t wavelengthIndex)
    {
        double cellT = CBaseCell::T_dir_dir(t_Side, t_IncomingDirection);
        double cellR = CBaseCell::R_dir_dir(t_Side, t_IncomingDirection);
        auto materialValue = m_Material->getBandProperty(
          FenestrationCommon::Property::R, t_Side, wavelengthIndex, t_IncomingDirection, t_OutgoingDirection);
        return cellR + (1 - cellT) * materialValue;
    }

}   // namespace SingleLayerOptics
