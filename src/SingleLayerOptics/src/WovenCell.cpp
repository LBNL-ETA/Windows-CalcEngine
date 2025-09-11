#include <cmath>
#include <algorithm>
#include <cassert>
#include <ranges>

#include "WovenCell.hpp"
#include "CellDescription.hpp"
#include "WovenCellDescription.hpp"
#include "MaterialDescription.hpp"
#include "WCECommon.hpp"
#include "BeamDirection.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    namespace Helper
    {
        double Tscatter(const double gamma, const CBeamDirection & t_Direction, const double Rmat)
        {
            double Tsct{0};
            if(Rmat > 0)
            {
                const double aAlt = degrees(t_Direction.Altitude());
                const double aAzm = degrees(t_Direction.Azimuth());

                if(gamma < 1)
                {
                    const double Tscattermax =
                      0.0229 * gamma + 0.2971 * Rmat - 0.03624 * std::pow(gamma, 2)
                      + 0.04763 * std::pow(Rmat, 2) - 0.44416 * gamma * Rmat;
                    const double DeltaMax = 89.7 - 10 * gamma / 0.16;
                    const double Delta = std::pow(std::pow(aAlt, 2) + std::pow(aAzm, 2), 0.5);
                    const double PeakRatio = 1 / (0.2 * Rmat * (1 - gamma));

                    double E = 0;
                    if(Delta > DeltaMax)
                    {
                        E = -(std::pow(std::abs(Delta - DeltaMax), 2.5)) / 600;
                        Tsct = -0.2 * Rmat * Tscattermax * (1 - gamma)
                               * std::max(0.0, (Delta - DeltaMax) / (90 - DeltaMax));
                    }
                    else
                    {
                        E = -(std::pow(std::abs(Delta - DeltaMax), 2)) / 600;
                        Tsct = 0;
                    }
                    Tsct = Tsct
                           + 0.2 * Rmat * Tscattermax * (1 - gamma)
                               * (1 + (PeakRatio - 1) * std::exp(E));
                }

                if(Tsct < 0)
                {
                    Tsct = 0;
                }
            }

            return Tsct;
        }
    }   // namespace Helper

    ////////////////////////////////////////////////////////////////////////////////////////////
    //  CWovenCell
    ////////////////////////////////////////////////////////////////////////////////////////////
    CWovenCell::CWovenCell(const std::shared_ptr<CMaterial> & t_MaterialProperties,
                           const std::shared_ptr<ICellDescription> & t_Cell) :
        CBaseCell(t_MaterialProperties, t_Cell), CUniformDiffuseCell(t_MaterialProperties, t_Cell)
    {}

    double CWovenCell::T_dir_dif(const Side t_Side, const CBeamDirection & t_Direction)
    {
        const double T_material = CUniformDiffuseCell::T_dir_dif(t_Side, t_Direction);
        const auto openness{CUniformDiffuseCell::T_dir_dir(t_Side, t_Direction)};
        const double Tsct = Tscatter_single(t_Side, t_Direction);
        return T_material * (1 - openness) + Tsct;
    }

    double CWovenCell::R_dir_dif(const Side t_Side, const CBeamDirection & t_Direction)
    {
        double R_material = CUniformDiffuseCell::R_dir_dif(t_Side, t_Direction);
        double Tsct = Tscatter_single(t_Side, t_Direction);
        return R_material - Tsct;
    }

    std::vector<double> CWovenCell::T_dir_dif_band(const Side t_Side,
                                                   const CBeamDirection & t_Direction)
    {
        // Get both band vectors once
        auto material =
          CUniformDiffuseCell::T_dir_dif_band(t_Side, t_Direction);   // vector<double>
        auto scatter = T_scatter_band(t_Side, t_Direction);           // vector<double>

        // Stop at the shorter just in case (they should match)
        const auto n = std::min(material.size(), scatter.size());
        material.resize(n);

        // In-place element-wise sum via zip
        for(auto && [m, s] : common::views::zip(material, scatter))
        {
            m += s;
        }

        return material;   // NRVO/move
    }

    double CWovenCell::T_dir_dif_at_wavelength(FenestrationCommon::Side t_Side,
                                               const CBeamDirection & t_Direction,
                                               size_t wavelengthIndex)
    {
        const double Tmaterial =
          CUniformDiffuseCell::T_dir_dif_at_wavelength(t_Side, t_Direction, wavelengthIndex);
        const double Tsct = Tscatter_at_wavelength(t_Side, t_Direction, wavelengthIndex);
        return Tmaterial + Tsct;
    }

    std::vector<double> CWovenCell::R_dir_dif_band(const Side t_Side,
                                                   const CBeamDirection & t_Direction)
    {
        const size_t size{m_Material->getBandSize()};
        std::vector<double> result;
        result.reserve(size);
        for(size_t i = 0; i < size; ++i)
        {
            result.push_back(R_dir_dif_at_wavelength(t_Side, t_Direction, i));
        }
        return result;
    }

    double CWovenCell::R_dir_dif_at_wavelength(Side t_Side,
                                               const CBeamDirection & t_Direction,
                                               size_t wavelengthIndex)
    {
        const double Rmaterial =
          CUniformDiffuseCell::R_dir_dif_at_wavelength(t_Side, t_Direction, wavelengthIndex);
        const double Tsct = Tscatter_at_wavelength(t_Side, t_Direction, wavelengthIndex);
        return Rmaterial - Tsct;
    }

    std::shared_ptr<CWovenCellDescription> CWovenCell::getCellAsWoven() const
    {
        if(std::dynamic_pointer_cast<CWovenCellDescription>(m_CellDescription) == nullptr)
        {
            assert("Incorrectly assigned cell description.");
        }

        std::shared_ptr<CWovenCellDescription> aCell =
          std::dynamic_pointer_cast<CWovenCellDescription>(m_CellDescription);

        return aCell;
    }

    double CWovenCell::Tscatter_single(const Side t_Side, const CBeamDirection & t_Direction) const
    {
        // Get matterial property from the opposite side of woven thread
        Side aScatterSide = oppositeSide(t_Side);
        double RScatter_mat = m_Material->getProperty(Property::R, aScatterSide);
        return Helper::Tscatter(getCellAsWoven()->gamma(), t_Direction, RScatter_mat);
    }

    std::vector<double> CWovenCell::T_scatter_band(const Side t_Side,
                                                   const CBeamDirection & t_Direction) const
    {
        auto RScatterMat{m_Material->getBandProperties(Property::R, oppositeSide(t_Side))};
        std::vector<double> result(RScatterMat.size());
        std::ranges::transform(RScatterMat, result.begin(), [&](double r) {
            return Helper::Tscatter(getCellAsWoven()->gamma(), t_Direction, r);
        });
        return result;
    }

    double CWovenCell::Tscatter_at_wavelength(Side t_Side,
                                              const CBeamDirection & t_Direction,
                                              size_t i) const
    {
        const auto & Rband = m_Material->getBandProperties(Property::R, oppositeSide(t_Side));
        return Helper::Tscatter(getCellAsWoven()->gamma(), t_Direction, Rband[i]);
    }
}   // namespace SingleLayerOptics
