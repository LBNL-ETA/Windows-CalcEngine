#include <cassert>

#include "SpecularCell.hpp"
#include "MaterialDescription.hpp"
#include "SpecularCellDescription.hpp"
#include "BeamDirection.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;
using namespace SpectralAveraging;

namespace SingleLayerOptics
{
    CSpecularCell::CSpecularCell(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                                 const std::shared_ptr<ICellDescription> & t_Cell) :
        CBaseCell(t_MaterialProperties, t_Cell)
    {}

    CSpecularCell::CSpecularCell(const std::shared_ptr<CMaterial> & t_MaterialProperties) :
        CBaseCell(t_MaterialProperties, std::make_shared<CSpecularCellDescription>())
    {}

    double CSpecularCell::T_dir_dir(const Side t_Side, const CBeamDirection & t_Direction)
    {
        // Specular glass transmittance is same for front and back sides
        return m_Material->getProperty(Property::T, t_Side, t_Direction);
    }

    double CSpecularCell::R_dir_dir(const Side t_Side, const CBeamDirection & t_Direction)
    {
        return m_Material->getProperty(Property::R, t_Side, t_Direction);
    }

    // Transmittance of specular material for each of material range. In case of sample measured
    // material, that are properties at each wavelength. In case of some other material type, that
    // will be properties for each of material ranges
    std::vector<double> CSpecularCell::T_dir_dir_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        return m_Material->getBandProperties(Property::T, t_Side, t_Direction);
    }

    double CSpecularCell::T_dir_dir_at_wavelength(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction,
                                                  size_t wavelengthIndex)
    {
        return m_Material->getBandProperty(Property::T, t_Side, wavelengthIndex, t_Direction);
    }

    // Reflectance of specular material over entire material range
    std::vector<double> CSpecularCell::R_dir_dir_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        return m_Material->getBandProperties(Property::R, t_Side, t_Direction);
    }

    std::shared_ptr<CSpecularCellDescription> CSpecularCell::getCellAsSpecular() const
    {
        if(std::dynamic_pointer_cast<CSpecularCellDescription>(m_CellDescription) == nullptr)
        {
            assert("Incorrectly assigned cell description.");
        }

        std::shared_ptr<CSpecularCellDescription> aCell =
          std::dynamic_pointer_cast<CSpecularCellDescription>(m_CellDescription);

        return aCell;
    }

    double CSpecularCell::R_dir_dir_at_wavelength(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction,
                                                  size_t wavelengthIndex)
    {
        return m_Material->getBandProperty(Property::R, t_Side, wavelengthIndex, t_Direction);
    }

}   // namespace SingleLayerOptics
