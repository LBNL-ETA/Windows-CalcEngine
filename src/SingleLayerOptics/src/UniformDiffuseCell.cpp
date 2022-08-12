#include "UniformDiffuseCell.hpp"
#include "MaterialDescription.hpp"
#include "CellDescription.hpp"
#include "BeamDirection.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CUniformDiffuseCell::CUniformDiffuseCell(
      const std::shared_ptr<CMaterial> & t_MaterialProperties,
      const std::shared_ptr<ICellDescription> & t_Cell,
      double rotation) :
        CBaseCell(t_MaterialProperties, t_Cell, rotation)
    {}

    double CUniformDiffuseCell::T_dir_dif(const Side t_Side, const CBeamDirection & t_Direction)
    {
        return getMaterialProperty(Property::T, t_Side, t_Direction);
    }

    double CUniformDiffuseCell::R_dir_dif(const Side t_Side, const CBeamDirection & t_Direction)
    {
        return ((1 - T_dir_dir(t_Side, t_Direction))
                * m_Material->getProperty(Property::R, t_Side));
    }

    std::vector<double> CUniformDiffuseCell::T_dir_dif_band(const Side t_Side,
                                                            const CBeamDirection & t_Direction)
    {
        return getMaterialProperties(Property::T, t_Side, t_Direction);
    }

    double CUniformDiffuseCell::T_dir_dif_at_wavelength(Side t_Side,
                                                        const CBeamDirection & t_Direction,
                                                        size_t wavelengthIndex)
    {
        return getMaterialPropertyAtWavelength(Property::T, t_Side, t_Direction, wavelengthIndex);
    }

    std::vector<double> CUniformDiffuseCell::R_dir_dif_band(const Side t_Side,
                                                            const CBeamDirection & t_Direction)
    {
        return getMaterialProperties(Property::R, t_Side, t_Direction);
    }

    double CUniformDiffuseCell::R_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                                        const CBeamDirection & t_Direction,
                                                        size_t wavelengthIndex)
    {
        return getMaterialPropertyAtWavelength(Property::R, t_Side, t_Direction, wavelengthIndex);
    }

    double CUniformDiffuseCell::getMaterialProperty(const Property t_Property,
                                                    const Side t_Side,
                                                    const CBeamDirection & t_Direction)
    {
        return ((1 - T_dir_dir(t_Side, t_Direction)) * m_Material->getProperty(t_Property, t_Side));
    }

    std::vector<double> CUniformDiffuseCell::getMaterialProperties(
      const Property t_Property, const Side t_Side, const CBeamDirection & t_Direction)
    {
        double materialCoverFraction = 1 - T_dir_dir(t_Side, t_Direction);
        std::vector<double> aMaterialProperties = m_Material->getBandProperties(t_Property, t_Side);
        std::vector<double> aProperty;
        aProperty.reserve(aMaterialProperties.size());
        for(const auto & materialProperty : aMaterialProperties)
        {
            aProperty.push_back(materialCoverFraction * materialProperty);
        }
        return aProperty;
    }

    double CUniformDiffuseCell::getMaterialPropertyAtWavelength(
      const FenestrationCommon::Property t_Property,
      const FenestrationCommon::Side t_Side,
      const CBeamDirection & t_Direction,
      size_t wavelengthIndex)
    {
        double materialCoverFraction = 1 - T_dir_dir(t_Side, t_Direction);
        return materialCoverFraction
               * m_Material->getBandProperty(t_Property, t_Side, wavelengthIndex);
    }
}   // namespace SingleLayerOptics
