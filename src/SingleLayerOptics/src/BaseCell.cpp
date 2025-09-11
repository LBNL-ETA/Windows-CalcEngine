#include <cassert>

#include <WCECommon.hpp>

#include "BaseCell.hpp"
#include "CellDescription.hpp"
#include "MaterialDescription.hpp"

using FenestrationCommon::Side;
using FenestrationCommon::CSeries;

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
            return m_CellDescription->Beam_dir_dir(t_Side, t_Direction.rotate(m_CellRotation));
        }
        return m_CellDescription->Beam_dir_dir(t_Side, t_Direction);
    }

    double CBaseCell::R_dir_dir(const Side, const CBeamDirection &)
    {
        return 0;
    }

    double CBaseCell::T_dir_dir_at_wavelength(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection & t_Direction,
                                              size_t wavelengthIndex)
    {
        std::ignore = wavelengthIndex;
        return T_dir_dir(t_Side, t_Direction);
    }

    double CBaseCell::R_dir_dir_at_wavelength(const FenestrationCommon::Side t_Side,
                                              const CBeamDirection & t_Direction,
                                              size_t wavelengthIndex)
    {
        std::ignore = wavelengthIndex;
        return R_dir_dir(t_Side, t_Direction);
    }

    template<class F>
    std::vector<double> CBaseCell::makeBand(F && f)
    {
        const size_t n = m_Material->getBandSize();
        std::vector<double> out;
        out.reserve(n);
        for(size_t i = 0; i < n; ++i)
            out.push_back(f(i));
        return out;
    }

    std::vector<double> CBaseCell::T_dir_dir_band(const Side t_Side,
                                                  const CBeamDirection & t_Direction)
    {
        return makeBand(
          [&](const size_t index) { return T_dir_dir_at_wavelength(t_Side, t_Direction, index); });
    }

    std::vector<double> CBaseCell::R_dir_dir_band(const Side t_Side,
                                                  const CBeamDirection & t_Direction)
    {
        return makeBand(
          [&](const size_t index) { return R_dir_dir_at_wavelength(t_Side, t_Direction, index); });
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

    void CBaseCell::Flipped(bool flipped)
    {
        m_Material->Flipped(flipped);
    }

    std::shared_ptr<CMaterial> CBaseCell::getMaterial()
    {
        return m_Material;
    }
}   // namespace SingleLayerOptics
