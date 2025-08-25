#include <cassert>

#include <WCECommon.hpp>

#include "BaseCell.hpp"
#include "CellDescription.hpp"
#include "MaterialDescription.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CBaseCell::CBaseCell() : m_Material(nullptr), m_CellDescription(nullptr), m_CellRotation(0)
    {}

    CBaseCell::CBaseCell(const std::shared_ptr<CMaterial> & t_Material,
                         const std::shared_ptr<ICellDescription> & t_CellDescription,
                         const double rotation) :
        m_Material(t_Material), m_CellDescription(t_CellDescription), m_CellRotation(rotation)
    {}

    void CBaseCell::setSourceData(const CSeries & t_SourceData)
    {
        m_Material->setSourceData(t_SourceData);
    }

    double CBaseCell::T_dir_dir(const Side t_Side, const CBeamDirection & t_Direction)
    {
        if(m_CellRotation != 0)
        {
            return m_CellDescription->T_dir_dir(t_Side, t_Direction.rotate(m_CellRotation));
        }
        return m_CellDescription->T_dir_dir(t_Side, t_Direction);
    }

    double CBaseCell::R_dir_dir(const Side t_Side, const CBeamDirection & t_Direction)
    {
        if(m_CellRotation != 0)
        {
            return m_CellDescription->R_dir_dir(t_Side, t_Direction.rotate(m_CellRotation));
        }
        return m_CellDescription->R_dir_dir(t_Side, t_Direction);
    }

    std::vector<double> CBaseCell::T_dir_dir_band(const Side t_Side,
                                                  const CBeamDirection & t_Direction)
    {
        const size_t size = m_Material->getBandSize();
        std::vector<double> aResults;
        aResults.reserve(size);
        for(size_t i = 0; i < size; i++)
        {
            aResults.push_back(T_dir_dir_at_wavelength(t_Side, t_Direction, i));
        }

        return aResults;
    }

    double CBaseCell::T_dir_dir_at_wavelength(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection & t_Direction,
                                              size_t wavelengthIndex)
    {
        std::ignore = wavelengthIndex;
        return T_dir_dir(t_Side, t_Direction);
    }

    std::vector<double> CBaseCell::R_dir_dir_band(const Side t_Side,
                                                  const CBeamDirection & t_Direction)
    {
        const size_t size = m_Material->getBandSize();
        std::vector<double> aResults;
        aResults.reserve(size);
        for(size_t i = 0; i < size; i++)
        {
            aResults.push_back(R_dir_dir_at_wavelength(t_Side, t_Direction, i));
        }

        return aResults;
    }

    double CBaseCell::R_dir_dir_at_wavelength(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection & t_Direction,
                                              size_t wavelengthIndex)
    {
        std::ignore = wavelengthIndex;
        return R_dir_dir(t_Side, t_Direction);
    }

    std::vector<double> CBaseCell::getBandWavelengths() const
    {
        assert(m_Material != nullptr);
        return m_Material->getBandWavelengths();
    }

    void CBaseCell::setBandWavelengths(const std::vector<double> & wavelengths)
    {
        assert(m_Material != nullptr);
        m_Material->setBandWavelengths(wavelengths);
    }

    int CBaseCell::getBandIndex(const double t_Wavelength) const
    {
        return m_Material->getBandIndex(t_Wavelength);
    }

    size_t CBaseCell::getBandSize() const
    {
        return m_Material->getBandSize();
    }

    double CBaseCell::getMinLambda() const
    {
        return m_Material->getMinLambda();
    }

    double CBaseCell::getMaxLambda() const
    {
        return m_Material->getMaxLambda();
    }

    void CBaseCell::Flipped(bool flipped) const
    {
        m_Material->Flipped(flipped);
    }

    std::shared_ptr<CMaterial> CBaseCell::getMaterial()
    {
        return m_Material;
    }
}   // namespace SingleLayerOptics
