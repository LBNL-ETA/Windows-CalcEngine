#include "MaterialDirDifCell.hpp"

#include "MaterialDescription.hpp"

namespace SingleLayerOptics
{
    using FenestrationCommon::Property;
    using FenestrationCommon::Side;

    CMaterialDirectionalDiffuseCell::CMaterialDirectionalDiffuseCell(
      const std::shared_ptr<CMaterial> & t_MaterialProperties,
      const std::shared_ptr<ICellDescription> & t_Cell) :
        CBaseCell(t_MaterialProperties, t_Cell, 0),
        CDirectionalDiffuseCell(t_MaterialProperties, t_Cell, 0)
    {}

    double CMaterialDirectionalDiffuseCell::T_dir_dir(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        return m_Material ? m_Material->getProperty(Property::T, t_Side, t_Direction, t_Direction)
                          : 0;
    }

    double CMaterialDirectionalDiffuseCell::R_dir_dir(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        return m_Material ? m_Material->getProperty(Property::R, t_Side, t_Direction, t_Direction)
                          : 0;
    }

    std::vector<double>
      CMaterialDirectionalDiffuseCell::T_dir_dir_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        return m_Material
                 ? m_Material->getBandProperties(Property::T, t_Side, t_Direction, t_Direction)
                 : std::vector<double>();
    }

    double CMaterialDirectionalDiffuseCell::T_dir_dir_at_wavelength(
      const Side t_Side, const CBeamDirection & t_Direction, const size_t wavelengthIndex)
    {
        return T_dir_dir_band(t_Side, t_Direction)[wavelengthIndex];
    }

    std::vector<double>
      CMaterialDirectionalDiffuseCell::R_dir_dir_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        return m_Material
                 ? m_Material->getBandProperties(Property::R, t_Side, t_Direction, t_Direction)
                 : std::vector<double>();
    }

    double CMaterialDirectionalDiffuseCell::R_dir_dir_at_wavelength(
      const Side t_Side, const CBeamDirection & t_Direction, const size_t wavelengthIndex)
    {
        return R_dir_dir_band(t_Side, t_Direction)[wavelengthIndex];
    }

    double CMaterialDirectionalDiffuseCell::T_dir_dif(const Side t_Side,
                                                      const CBeamDirection & t_IncomingDirection,
                                                      const CBeamDirection & t_OutgoingDirection)
    {
        return m_Material && t_IncomingDirection != t_OutgoingDirection
                 ? m_Material->getProperty(
                     Property::T, t_Side, t_IncomingDirection, t_OutgoingDirection)
                 : 0;
    }

    double CMaterialDirectionalDiffuseCell::R_dir_dif(const Side t_Side,
                                                      const CBeamDirection & t_IncomingDirection,
                                                      const CBeamDirection & t_OutgoingDirection)
    {
        return m_Material && t_IncomingDirection != t_OutgoingDirection ? m_Material->getProperty(
                              Property::R, t_Side, t_IncomingDirection, t_OutgoingDirection)
                          : 0;
    }

    std::vector<double>
      CMaterialDirectionalDiffuseCell::T_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_IncomingDirection,
                                                      const CBeamDirection & t_OutgoingDirection)
    {
        return m_Material && t_IncomingDirection != t_OutgoingDirection ? m_Material->getBandProperties(
                              Property::T, t_Side, t_IncomingDirection, t_OutgoingDirection)
                          : std::vector<double>();
    }

    double CMaterialDirectionalDiffuseCell::T_dir_dif_by_wavelength(
      const Side t_Side,
      const CBeamDirection & t_IncomingDirection,
      const CBeamDirection & t_OutgoingDirection,
      const size_t wavelengthIndex)
    {
        return m_Material && t_IncomingDirection != t_OutgoingDirection
                 ? m_Material->getBandProperties(
                     Property::T, t_Side, t_IncomingDirection, t_OutgoingDirection)[wavelengthIndex]
                 : 0;
    }

    std::vector<double>
      CMaterialDirectionalDiffuseCell::R_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_IncomingDirection,
                                                      const CBeamDirection & t_OutgoingDirection)
    {
        return m_Material && t_IncomingDirection != t_OutgoingDirection ? m_Material->getBandProperties(
                              Property::R, t_Side, t_IncomingDirection, t_OutgoingDirection)
                          : std::vector<double>();
    }

    double CMaterialDirectionalDiffuseCell::R_dir_dif_by_wavelength(
      const Side t_Side,
      const CBeamDirection & t_IncomingDirection,
      const CBeamDirection & t_OutgoingDirection,
      const size_t wavelengthIndex)
    {
        return m_Material && t_IncomingDirection != t_OutgoingDirection
                 ? m_Material->getBandProperties(
                     Property::R, t_Side, t_IncomingDirection, t_OutgoingDirection)[wavelengthIndex]
                 : 0;
    }
}   // namespace SingleLayerOptics