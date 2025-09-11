#include <cassert>

#include "VenetianCell.hpp"
#include "VenetianCellDescription.hpp"
#include "BeamDirection.hpp"
#include "MaterialDescription.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    //  CVenetianBase
    ////////////////////////////////////////////////////////////////////////////////////////////

    CVenetianBase::CVenetianBase(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                                 const std::shared_ptr<ICellDescription> & t_Cell,
                                 double rotation) :
        CUniformDiffuseCell(t_MaterialProperties, t_Cell, rotation),
        CDirectionalDiffuseCell(t_MaterialProperties, t_Cell, rotation)
    {}

    std::shared_ptr<CVenetianCellDescription> CVenetianBase::getCellAsVenetian() const
    {
        if(std::dynamic_pointer_cast<CVenetianCellDescription>(m_CellDescription) == nullptr)
        {
            assert("Incorrectly assigned cell description.");
        }

        std::shared_ptr<CVenetianCellDescription> aCell =
          std::dynamic_pointer_cast<CVenetianCellDescription>(m_CellDescription);

        return aCell;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////
    //  CVenetianCell
    ////////////////////////////////////////////////////////////////////////////////////////////
    CVenetianCell::CVenetianCell(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                                 const std::shared_ptr<ICellDescription> & t_Cell,
                                 const double rotation) :
        CBaseCell(t_MaterialProperties, t_Cell, rotation),
        CVenetianBase(t_MaterialProperties, t_Cell, rotation),
        m_BackwardFlowCellDescription(getCellAsVenetian()->getBackwardFlowCell())
    {
        assert(t_Cell != nullptr);
        assert(t_MaterialProperties != nullptr);

        generateVenetianEnergy();
    }

    void CVenetianCell::generateVenetianEnergy()
    {
        const auto & venetianForwardGeometry{getCellAsVenetian()};
        m_Energy =
          CVenetianEnergy(*m_Material, venetianForwardGeometry, m_BackwardFlowCellDescription);
        // Create energy states for entire material band
        m_EnergiesBand.clear();
        std::vector<RMaterialProperties> aMat = m_Material->getBandProperties();

        if(!aMat.empty())
        {
            size_t size = m_Material->getBandSize();
            for(size_t i = 0; i < size; ++i)
            {
                double Tf = aMat[i].getProperty(Property::T, Side::Front);
                double Tb = aMat[i].getProperty(Property::T, Side::Back);
                double Rf = aMat[i].getProperty(Property::R, Side::Front);
                double Rb = aMat[i].getProperty(Property::R, Side::Back);

                m_EnergiesBand.emplace_back(LayerProperties{Tf, Rf, Tb, Rb},
                                            venetianForwardGeometry,
                                            m_BackwardFlowCellDescription);
            }
        }
    }

    void CVenetianCell::setSourceData(const CSeries & t_SourceData)
    {
        CBaseCell::setSourceData(t_SourceData);
        generateVenetianEnergy();
    }

    void CVenetianCell::setBandWavelengths(const std::vector<double> & wavelengths)
    {
        CBaseCell::setBandWavelengths(wavelengths);
        generateVenetianEnergy();
    }

    double CVenetianCell::T_dir_dir(const Side t_Side, const CBeamDirection & t_Direction)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).T_dir_dir(t_Direction.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).T_dir_dir(t_Direction);
    }

    std::vector<double> CVenetianCell::T_dir_dir_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(T_dir_dir_at_wavelength(t_Side, t_Direction, i));
        }
        return aProperties;
    }

    double CVenetianCell::T_dir_dir_at_wavelength(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dir(
              t_Direction.rotate(m_CellRotation));
        }

        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dir(t_Direction);
    }

    double CVenetianCell::T_dir_dif(const Side t_Side, const CBeamDirection & t_Direction)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).T_dir_dif(t_Direction.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).T_dir_dif(t_Direction);
    }

    std::vector<double> CVenetianCell::T_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(T_dir_dif_at_wavelength(t_Side, t_Direction, i));
        }
        return aProperties;
    }

    double CVenetianCell::T_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dif(
              t_Direction.rotate(m_CellRotation));
        }

        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dif(t_Direction);
    }

    double CVenetianCell::R_dir_dif(const Side t_Side, const CBeamDirection & t_Direction)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).R_dir_dif(t_Direction.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).R_dir_dif(t_Direction);
    }

    std::vector<double> CVenetianCell::R_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_Direction)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(R_dir_dif_at_wavelength(t_Side, t_Direction, i));
        }
        return aProperties;
    }

    double CVenetianCell::R_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_Direction,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).R_dir_dif(
              t_Direction.rotate(m_CellRotation));
        }

        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).R_dir_dif(t_Direction);
    }

    double CVenetianCell::T_dir_dif(const Side t_Side,
                                    const CBeamDirection & t_IncomingDirection,
                                    const CBeamDirection & t_OutgoingDirection)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).T_dir_dir(t_IncomingDirection.rotate(m_CellRotation),
                                                      t_OutgoingDirection.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).T_dir_dir(t_IncomingDirection, t_OutgoingDirection);
    }

    std::vector<double> CVenetianCell::T_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_IncomingDirection,
                                                      const CBeamDirection & t_OutgoingDirection)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(
              T_dir_dif_by_wavelength(t_Side, t_IncomingDirection, t_OutgoingDirection, i));
        }
        return aProperties;
    }

    double CVenetianCell::T_dir_dif_by_wavelength(const FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_IncomingDirection,
                                                  const CBeamDirection & t_OutgoingDirection,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dir(
              t_IncomingDirection.rotate(m_CellRotation),
              t_OutgoingDirection.rotate(m_CellRotation));
        }
        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).T_dir_dir(t_IncomingDirection,
                                                                         t_OutgoingDirection);
    }

    double CVenetianCell::R_dir_dif(const Side t_Side,
                                    const CBeamDirection & t_IncomingDirection,
                                    const CBeamDirection & t_OutgoingDirection)
    {
        if(m_CellRotation != 0)
        {
            return m_Energy.getCell(t_Side).R_dir_dir(t_IncomingDirection.rotate(m_CellRotation),
                                                      t_OutgoingDirection.rotate(m_CellRotation));
        }
        return m_Energy.getCell(t_Side).R_dir_dir(t_IncomingDirection, t_OutgoingDirection);
    }

    std::vector<double> CVenetianCell::R_dir_dif_band(const Side t_Side,
                                                      const CBeamDirection & t_IncomingDirection,
                                                      const CBeamDirection & t_OutgoingDirection)
    {
        size_t size = m_EnergiesBand.size();
        std::vector<double> aProperties;
        aProperties.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            aProperties.push_back(
              R_dir_dif_by_wavelength(t_Side, t_IncomingDirection, t_OutgoingDirection, i));
        }
        return aProperties;
    }

    double CVenetianCell::R_dir_dif_by_wavelength(const FenestrationCommon::Side t_Side,
                                                  const CBeamDirection & t_IncomingDirection,
                                                  const CBeamDirection & t_OutgoingDirection,
                                                  size_t wavelengthIndex)
    {
        if(m_CellRotation != 0)
        {
            return m_EnergiesBand[wavelengthIndex].getCell(t_Side).R_dir_dir(
              t_IncomingDirection.rotate(m_CellRotation),
              t_OutgoingDirection.rotate(m_CellRotation));
        }

        return m_EnergiesBand[wavelengthIndex].getCell(t_Side).R_dir_dir(t_IncomingDirection,
                                                                         t_OutgoingDirection);
    }

    double CVenetianCell::T_dif_dif(const Side t_Side)
    {
        return m_Energy.getCell(t_Side).T_dif_dif();
    }

    double CVenetianCell::R_dif_dif(const Side t_Side)
    {
        return m_Energy.getCell(t_Side).R_dif_dif();
    }
}   // namespace SingleLayerOptics
