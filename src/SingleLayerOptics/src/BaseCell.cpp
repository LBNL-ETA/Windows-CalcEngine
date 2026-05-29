#include <algorithm>
#include <cassert>
#include <cmath>
#include <ranges>
#include <stdexcept>
#include <utility>

#include <WCECommon.hpp>

#include "BaseCell.hpp"
#include "BeamDirection.hpp"
#include "MaterialDescription.hpp"
#include "WovenCellDescription.hpp"

using FenestrationCommon::CSeries;
using FenestrationCommon::oppositeSide;
using FenestrationCommon::Property;
using FenestrationCommon::Side;
using FenestrationCommon::degrees;

namespace SingleLayerOptics
{
    namespace
    {
        //! Apply cell rotation if the cell carries one, otherwise pass the direction through.
        [[nodiscard]] CBeamDirection rotateIfNeeded(double rotation, CBeamDirection const & dir)
        {
            if(rotation == 0.0)
            {
                return dir;
            }
            return dir.rotate(rotation);
        }

        //! Tscatter for woven shades. Same formula as the pre-Phase-A CWovenCell helper.
        [[nodiscard]] double wovenTscatter(double gamma,
                                           CBeamDirection const & direction,
                                           double rMat)
        {
            if(rMat <= 0)
            {
                return 0;
            }
            if(gamma >= 1)
            {
                return 0;
            }

            double const aAlt = degrees(direction.Altitude());
            double const aAzm = degrees(direction.Azimuth());

            double const tScatterMax = 0.0229 * gamma + 0.2971 * rMat - 0.03624 * std::pow(gamma, 2)
                                       + 0.04763 * std::pow(rMat, 2) - 0.44416 * gamma * rMat;
            double const deltaMax = 89.7 - 10 * gamma / 0.16;
            double const delta = std::pow(std::pow(aAlt, 2) + std::pow(aAzm, 2), 0.5);
            double const peakRatio = 1 / (0.2 * rMat * (1 - gamma));

            double exponent = 0;
            double tSct = 0;
            if(delta > deltaMax)
            {
                exponent = -(std::pow(std::abs(delta - deltaMax), 2.5)) / 600;
                tSct = -0.2 * rMat * tScatterMax * (1 - gamma)
                       * std::max(0.0, (delta - deltaMax) / (90 - deltaMax));
            }
            else
            {
                exponent = -(std::pow(std::abs(delta - deltaMax), 2)) / 600;
            }
            tSct = tSct
                   + 0.2 * rMat * tScatterMax * (1 - gamma) * (1 + (peakRatio - 1) * std::exp(exponent));

            return tSct < 0 ? 0 : tSct;
        }

        [[nodiscard]] double wovenGamma(CellDescription const & description)
        {
            assert(std::holds_alternative<CWovenCellDescription>(description)
                   && "Woven cell must carry a CWovenCellDescription.");
            return std::get<CWovenCellDescription>(description).gamma();
        }

        //! wovenTscatter wrapped with the woven cell's gamma, for call sites that only
        //! have the description + direction + reflectance.
        [[nodiscard]] double wovenScatter(CellDescription const & description,
                                          CBeamDirection const & direction,
                                          double rScatter)
        {
            return wovenTscatter(wovenGamma(description), direction, rScatter);
        }

        //! Directional/Homogeneous blend: the cell's own component plus the material
        //! contribution scaled by the open fraction of the cell (1 - direct-direct T).
        [[nodiscard]] double combineDiffuse(double cellComponent,
                                            double cellT,
                                            double materialValue)
        {
            return cellComponent + (1.0 - cellT) * materialValue;
        }

        //! Build a per-wavelength band vector by evaluating perWavelength(idx) over the band.
        template<class PerWavelengthFn>
        [[nodiscard]] std::vector<double> buildBand(std::size_t bandSize,
                                                    PerWavelengthFn && perWavelength)
        {
            std::vector<double> out;
            out.reserve(bandSize);
            for(std::size_t idx = 0; idx < bandSize; ++idx)
            {
                out.push_back(perWavelength(idx));
            }
            return out;
        }
    }   // namespace

    CBaseCell::CBaseCell() : m_Material(nullptr), m_CellDescription(CSpecularCellDescription{})
    {}

    CBaseCell::CBaseCell(std::shared_ptr<CMaterial> material,
                         CellDescription description,
                         CellKind kind,
                         double rotation) :
        m_Material(std::move(material)),
        m_CellDescription(std::move(description)),
        m_Kind(kind),
        m_CellRotation(rotation)
    {
        if(m_Kind == CellKind::Venetian)
        {
            m_Venetian.emplace();
            generateVenetianEnergy();
        }
    }

    void CBaseCell::generateVenetianEnergy()
    {
        assert(m_Kind == CellKind::Venetian && m_Venetian.has_value());
        assert(std::holds_alternative<CVenetianCellDescription>(m_CellDescription)
               && "Venetian cell must carry a CVenetianCellDescription.");

        auto const & forward = std::get<CVenetianCellDescription>(m_CellDescription);
        auto forwardGeometry = makeVenetianGeometry(forward);
        auto backwardGeometry = makeVenetianGeometry(forward.getBackwardFlowCell());

        m_Venetian->m_Energy = CVenetianEnergy(*m_Material, forwardGeometry, backwardGeometry);
        m_Venetian->m_EnergiesBand.clear();

        auto const bandProperties = m_Material->getBandProperties();
        if(bandProperties.empty())
        {
            return;
        }

        std::size_t const bandSize = m_Material->getBandSize();
        m_Venetian->m_EnergiesBand.reserve(bandSize);
        for(std::size_t idx = 0; idx < bandSize; ++idx)
        {
            double const tFront = bandProperties[idx].getProperty(Property::T, Side::Front);
            double const tBack = bandProperties[idx].getProperty(Property::T, Side::Back);
            double const rFront = bandProperties[idx].getProperty(Property::R, Side::Front);
            double const rBack = bandProperties[idx].getProperty(Property::R, Side::Back);
            m_Venetian->m_EnergiesBand.emplace_back(
              LayerProperties{tFront, rFront, tBack, rBack}, forwardGeometry, backwardGeometry);
        }
    }

    void CBaseCell::setSourceData(CSeries const & sourceData)
    {
        m_Material->setSourceData(sourceData);
        if(m_Kind == CellKind::Venetian)
        {
            generateVenetianEnergy();
        }
    }

    void CBaseCell::setBandWavelengths(std::vector<double> const & wavelengths)
    {
        assert(m_Material != nullptr);
        m_Material->setBandWavelengths(wavelengths);
        if(m_Kind == CellKind::Venetian)
        {
            generateVenetianEnergy();
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    //  direct-direct
    //////////////////////////////////////////////////////////////////////////////////

    double CBaseCell::T_dir_dir(Side side, CBeamDirection const & dir)
    {
        switch(m_Kind)
        {
            case CellKind::Specular:
                return m_Material->getProperty(Property::T, side, dir, dir);
            case CellKind::MaterialDirectionalDiffuse:
                return m_Material ? m_Material->getProperty(Property::T, side, dir, dir) : 0;
            case CellKind::Venetian:
                return m_Venetian->m_Energy.getCell(side).T_dir_dir(
                  rotateIfNeeded(m_CellRotation, dir));
            default:
                return Beam_dir_dir(m_CellDescription, side, rotateIfNeeded(m_CellRotation, dir));
        }
    }

    double CBaseCell::R_dir_dir(Side side, CBeamDirection const & dir)
    {
        switch(m_Kind)
        {
            case CellKind::Specular:
                return m_Material->getProperty(Property::R, side, dir, dir);
            case CellKind::MaterialDirectionalDiffuse:
                return m_Material ? m_Material->getProperty(Property::R, side, dir, dir) : 0;
            default:
                return 0;
        }
    }

    double CBaseCell::T_dir_dir_at_wavelength(Side side,
                                              CBeamDirection const & dir,
                                              std::size_t wavelengthIndex)
    {
        switch(m_Kind)
        {
            case CellKind::Specular:
                return m_Material->getBandProperty(Property::T, side, wavelengthIndex, dir, dir);
            case CellKind::MaterialDirectionalDiffuse:
                if(!m_Material)
                {
                    return 0;
                }
                return m_Material->getBandProperties(Property::T, side, dir, dir)[wavelengthIndex];
            case CellKind::Venetian:
                return m_Venetian->m_EnergiesBand[wavelengthIndex].getCell(side).T_dir_dir(
                  rotateIfNeeded(m_CellRotation, dir));
            default:
                return T_dir_dir(side, dir);
        }
    }

    double CBaseCell::R_dir_dir_at_wavelength(Side side,
                                              CBeamDirection const & dir,
                                              std::size_t wavelengthIndex)
    {
        switch(m_Kind)
        {
            case CellKind::Specular:
                return m_Material->getBandProperty(Property::R, side, wavelengthIndex, dir, dir);
            case CellKind::MaterialDirectionalDiffuse:
                if(!m_Material)
                {
                    return 0;
                }
                return m_Material->getBandProperties(Property::R, side, dir, dir)[wavelengthIndex];
            default:
                return R_dir_dir(side, dir);
        }
    }

    std::vector<double> CBaseCell::T_dir_dir_band(Side side, CBeamDirection const & dir)
    {
        switch(m_Kind)
        {
            case CellKind::Specular:
                return m_Material->getBandProperties(Property::T, side, dir, dir);
            case CellKind::MaterialDirectionalDiffuse:
                if(!m_Material)
                {
                    return {};
                }
                return m_Material->getBandProperties(Property::T, side, dir, dir);
            default:
                return buildBand(m_Material->getBandSize(), [&](std::size_t idx) {
                    return T_dir_dir_at_wavelength(side, dir, idx);
                });
        }
    }

    std::vector<double> CBaseCell::R_dir_dir_band(Side side, CBeamDirection const & dir)
    {
        switch(m_Kind)
        {
            case CellKind::Specular:
                return m_Material->getBandProperties(Property::R, side, dir, dir);
            case CellKind::MaterialDirectionalDiffuse:
                if(!m_Material)
                {
                    return {};
                }
                return m_Material->getBandProperties(Property::R, side, dir, dir);
            default:
                return buildBand(m_Material->getBandSize(), [&](std::size_t idx) {
                    return R_dir_dir_at_wavelength(side, dir, idx);
                });
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    //  single-direction diffuse  (Uniform / Woven / Venetian-uniform)
    //////////////////////////////////////////////////////////////////////////////////

    double CBaseCell::T_dir_dif(Side side, CBeamDirection const & dir)
    {
        switch(m_Kind)
        {
            case CellKind::UniformDiffuse:
                return (1.0 - T_dir_dir(side, dir)) * m_Material->getProperty(Property::T, side);
            case CellKind::Woven:
            {
                double const cellT = T_dir_dir(side, dir);
                double const tMaterial = (1.0 - cellT) * m_Material->getProperty(Property::T, side);
                double const rScatterMat = m_Material->getProperty(Property::R, oppositeSide(side));
                double const tScatter =
                  wovenScatter(m_CellDescription, dir, rScatterMat);
                return tMaterial * (1.0 - cellT) + tScatter;
            }
            case CellKind::Venetian:
                return m_Venetian->m_Energy.getCell(side).T_dir_dif(
                  rotateIfNeeded(m_CellRotation, dir));
            default:
                return 0;
        }
    }

    double CBaseCell::R_dir_dif(Side side, CBeamDirection const & dir)
    {
        switch(m_Kind)
        {
            case CellKind::UniformDiffuse:
                return (1.0 - T_dir_dir(side, dir)) * m_Material->getProperty(Property::R, side);
            case CellKind::Woven:
            {
                double const rMaterial =
                  (1.0 - T_dir_dir(side, dir)) * m_Material->getProperty(Property::R, side);
                double const rScatterMat = m_Material->getProperty(Property::R, oppositeSide(side));
                double const tScatter =
                  wovenScatter(m_CellDescription, dir, rScatterMat);
                return rMaterial - tScatter;
            }
            case CellKind::Venetian:
                return m_Venetian->m_Energy.getCell(side).R_dir_dif(
                  rotateIfNeeded(m_CellRotation, dir));
            default:
                return 0;
        }
    }

    double CBaseCell::T_dir_dif_at_wavelength(Side side,
                                              CBeamDirection const & dir,
                                              std::size_t wavelengthIndex)
    {
        switch(m_Kind)
        {
            case CellKind::UniformDiffuse:
            {
                double const materialCover = 1.0 - T_dir_dir(side, dir);
                return materialCover
                       * m_Material->getBandProperty(Property::T, side, wavelengthIndex);
            }
            case CellKind::Woven:
            {
                double const tMaterial =
                  (1.0 - T_dir_dir(side, dir))
                  * m_Material->getBandProperty(Property::T, side, wavelengthIndex);
                double const rScatterAtWavelength =
                  m_Material->getBandProperties(Property::R, oppositeSide(side))[wavelengthIndex];
                double const tScatter =
                  wovenScatter(m_CellDescription, dir, rScatterAtWavelength);
                return tMaterial + tScatter;
            }
            case CellKind::Venetian:
                // Per-wavelength Venetian: use the band-indexed CVenetianEnergy whose
                // LayerProperties reflect the material's properties at this wavelength.
                return m_Venetian->m_EnergiesBand[wavelengthIndex].getCell(side).T_dir_dif(
                  rotateIfNeeded(m_CellRotation, dir));
            default:
                return T_dir_dif(side, dir);
        }
    }

    double CBaseCell::R_dir_dif_at_wavelength(Side side,
                                              CBeamDirection const & dir,
                                              std::size_t wavelengthIndex)
    {
        switch(m_Kind)
        {
            case CellKind::UniformDiffuse:
            {
                double const materialCover = 1.0 - T_dir_dir(side, dir);
                return materialCover
                       * m_Material->getBandProperty(Property::R, side, wavelengthIndex);
            }
            case CellKind::Woven:
            {
                double const rMaterial =
                  (1.0 - T_dir_dir(side, dir))
                  * m_Material->getBandProperty(Property::R, side, wavelengthIndex);
                double const rScatterAtWavelength =
                  m_Material->getBandProperties(Property::R, oppositeSide(side))[wavelengthIndex];
                double const tScatter =
                  wovenScatter(m_CellDescription, dir, rScatterAtWavelength);
                return rMaterial - tScatter;
            }
            case CellKind::Venetian:
                return m_Venetian->m_EnergiesBand[wavelengthIndex].getCell(side).R_dir_dif(
                  rotateIfNeeded(m_CellRotation, dir));
            default:
                return R_dir_dif(side, dir);
        }
    }

    std::vector<double> CBaseCell::T_dir_dif_band(Side side, CBeamDirection const & dir)
    {
        switch(m_Kind)
        {
            case CellKind::UniformDiffuse:
            {
                double const materialCover = 1.0 - T_dir_dir(side, dir);
                auto materialBand = m_Material->getBandProperties(Property::T, side);
                for(auto & value : materialBand)
                {
                    value *= materialCover;
                }
                return materialBand;
            }
            case CellKind::Woven:
            {
                // Element-wise sum of the Uniform single-direction band and the per-wavelength
                // Tscatter term.
                double const materialCover = 1.0 - T_dir_dir(side, dir);
                auto materialBand = m_Material->getBandProperties(Property::T, side);
                for(auto & value : materialBand)
                {
                    value *= materialCover;
                }
                auto rScatterBand = m_Material->getBandProperties(Property::R, oppositeSide(side));
                std::size_t const n = std::min(materialBand.size(), rScatterBand.size());
                for(std::size_t idx = 0; idx < n; ++idx)
                {
                    materialBand[idx] +=
                      wovenScatter(m_CellDescription, dir, rScatterBand[idx]);
                }
                return materialBand;
            }
            default:
                return buildBand(m_Material->getBandSize(), [&](std::size_t idx) {
                    return T_dir_dif_at_wavelength(side, dir, idx);
                });
        }
    }

    std::vector<double> CBaseCell::R_dir_dif_band(Side side, CBeamDirection const & dir)
    {
        switch(m_Kind)
        {
            case CellKind::UniformDiffuse:
            {
                double const materialCover = 1.0 - T_dir_dir(side, dir);
                auto materialBand = m_Material->getBandProperties(Property::R, side);
                for(auto & value : materialBand)
                {
                    value *= materialCover;
                }
                return materialBand;
            }
            default:
                return buildBand(m_Material->getBandSize(), [&](std::size_t idx) {
                    return R_dir_dif_at_wavelength(side, dir, idx);
                });
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
    //  two-direction diffuse  (Directional / Homogeneous / MaterialDirDif / Venetian-directional)
    //////////////////////////////////////////////////////////////////////////////////

    double CBaseCell::T_dir_dif(Side side,
                                CBeamDirection const & incoming,
                                CBeamDirection const & outgoing)
    {
        switch(m_Kind)
        {
            case CellKind::DirectionalDiffuse:
            {
                double const cellT = T_dir_dir(side, incoming);
                double const materialT =
                  m_Material->getProperty(Property::T, side, incoming, outgoing);
                return combineDiffuse(cellT, cellT, materialT);
            }
            case CellKind::HomogeneousDiffuse:
            {
                double const cellT = T_dir_dir(side, CBeamDirection());
                double const materialT = m_Material->getProperty(
                  Property::T, side, CBeamDirection(), CBeamDirection(),
                  OutgoingAggregation::Hemispherical);
                return combineDiffuse(cellT, cellT, materialT);
            }
            case CellKind::MaterialDirectionalDiffuse:
                if(!m_Material || incoming == outgoing)
                {
                    return 0;
                }
                return m_Material->getProperty(Property::T, side, incoming, outgoing);
            case CellKind::Venetian:
                return m_Venetian->m_Energy.getCell(side).T_dir_dir(
                  rotateIfNeeded(m_CellRotation, incoming),
                  rotateIfNeeded(m_CellRotation, outgoing));
            default:
                return 0;
        }
    }

    double CBaseCell::R_dir_dif(Side side,
                                CBeamDirection const & incoming,
                                CBeamDirection const & outgoing)
    {
        switch(m_Kind)
        {
            case CellKind::DirectionalDiffuse:
            {
                double const cellT = T_dir_dir(side, incoming);
                double const cellR = R_dir_dir(side, incoming);
                double const materialR =
                  m_Material->getProperty(Property::R, side, incoming, outgoing);
                return combineDiffuse(cellR, cellT, materialR);
            }
            case CellKind::HomogeneousDiffuse:
            {
                double const cellT = T_dir_dir(side, CBeamDirection());
                double const cellR = R_dir_dir(side, CBeamDirection());
                double const materialR = m_Material->getProperty(
                  Property::R, side, CBeamDirection(), CBeamDirection(),
                  OutgoingAggregation::Hemispherical);
                return combineDiffuse(cellR, cellT, materialR);
            }
            case CellKind::MaterialDirectionalDiffuse:
                if(!m_Material || incoming == outgoing)
                {
                    return 0;
                }
                return m_Material->getProperty(Property::R, side, incoming, outgoing);
            case CellKind::Venetian:
                return m_Venetian->m_Energy.getCell(side).R_dir_dir(
                  rotateIfNeeded(m_CellRotation, incoming),
                  rotateIfNeeded(m_CellRotation, outgoing));
            default:
                return 0;
        }
    }

    double CBaseCell::T_dir_dif_by_wavelength(Side side,
                                              CBeamDirection const & incoming,
                                              CBeamDirection const & outgoing,
                                              std::size_t wavelengthIndex)
    {
        switch(m_Kind)
        {
            case CellKind::DirectionalDiffuse:
            {
                double const cellT = T_dir_dir(side, incoming);
                double const materialT = m_Material->getBandProperty(
                  Property::T, side, wavelengthIndex, incoming, outgoing);
                return combineDiffuse(cellT, cellT, materialT);
            }
            case CellKind::HomogeneousDiffuse:
            {
                double const cellT = T_dir_dir(side, CBeamDirection());
                double const materialT = m_Material->getBandProperty(
                  Property::T, side, wavelengthIndex, CBeamDirection(), CBeamDirection(),
                  OutgoingAggregation::Hemispherical);
                return combineDiffuse(cellT, cellT, materialT);
            }
            case CellKind::MaterialDirectionalDiffuse:
                if(!m_Material || incoming == outgoing)
                {
                    return 0;
                }
                return m_Material
                  ->getBandProperties(Property::T, side, incoming, outgoing)[wavelengthIndex];
            case CellKind::Venetian:
                return m_Venetian->m_EnergiesBand[wavelengthIndex].getCell(side).T_dir_dir(
                  rotateIfNeeded(m_CellRotation, incoming),
                  rotateIfNeeded(m_CellRotation, outgoing));
            default:
                return 0;
        }
    }

    double CBaseCell::R_dir_dif_by_wavelength(Side side,
                                              CBeamDirection const & incoming,
                                              CBeamDirection const & outgoing,
                                              std::size_t wavelengthIndex)
    {
        switch(m_Kind)
        {
            case CellKind::DirectionalDiffuse:
            {
                double const cellT = T_dir_dir(side, incoming);
                double const cellR = R_dir_dir(side, incoming);
                double const materialR = m_Material->getBandProperty(
                  Property::R, side, wavelengthIndex, incoming, outgoing);
                return combineDiffuse(cellR, cellT, materialR);
            }
            case CellKind::HomogeneousDiffuse:
            {
                double const cellT = T_dir_dir(side, CBeamDirection());
                double const cellR = R_dir_dir(side, CBeamDirection());
                double const materialR = m_Material->getBandProperty(
                  Property::R, side, wavelengthIndex, CBeamDirection(), CBeamDirection(),
                  OutgoingAggregation::Hemispherical);
                return combineDiffuse(cellR, cellT, materialR);
            }
            case CellKind::MaterialDirectionalDiffuse:
                if(!m_Material || incoming == outgoing)
                {
                    return 0;
                }
                return m_Material
                  ->getBandProperties(Property::R, side, incoming, outgoing)[wavelengthIndex];
            case CellKind::Venetian:
                return m_Venetian->m_EnergiesBand[wavelengthIndex].getCell(side).R_dir_dir(
                  rotateIfNeeded(m_CellRotation, incoming),
                  rotateIfNeeded(m_CellRotation, outgoing));
            default:
                return 0;
        }
    }

    std::vector<double> CBaseCell::T_dir_dif_band(Side side,
                                                  CBeamDirection const & incoming,
                                                  CBeamDirection const & outgoing)
    {
        switch(m_Kind)
        {
            case CellKind::DirectionalDiffuse:
            {
                double const cellT = T_dir_dir(side, incoming);
                auto materialBand =
                  m_Material->getBandProperties(Property::T, side, incoming, outgoing);
                for(auto & value : materialBand)
                {
                    value = combineDiffuse(cellT, cellT, value);
                }
                return materialBand;
            }
            default:
                return buildBand(m_Material->getBandSize(), [&](std::size_t idx) {
                    return T_dir_dif_by_wavelength(side, incoming, outgoing, idx);
                });
        }
    }

    std::vector<double> CBaseCell::R_dir_dif_band(Side side,
                                                  CBeamDirection const & incoming,
                                                  CBeamDirection const & outgoing)
    {
        switch(m_Kind)
        {
            case CellKind::DirectionalDiffuse:
            {
                double const cellT = T_dir_dir(side, incoming);
                double const cellR = R_dir_dir(side, incoming);
                auto materialBand =
                  m_Material->getBandProperties(Property::R, side, incoming, outgoing);
                for(auto & value : materialBand)
                {
                    value = combineDiffuse(cellR, cellT, value);
                }
                return materialBand;
            }
            default:
                return buildBand(m_Material->getBandSize(), [&](std::size_t idx) {
                    return R_dir_dif_by_wavelength(side, incoming, outgoing, idx);
                });
        }
    }

    double CBaseCell::T_dif_dif(Side side)
    {
        assert(m_Kind == CellKind::Venetian && m_Venetian.has_value()
               && "T_dif_dif is only meaningful for Venetian cells.");
        return m_Venetian->m_Energy.getCell(side).T_dif_dif();
    }

    double CBaseCell::R_dif_dif(Side side)
    {
        assert(m_Kind == CellKind::Venetian && m_Venetian.has_value()
               && "R_dif_dif is only meaningful for Venetian cells.");
        return m_Venetian->m_Energy.getCell(side).R_dif_dif();
    }

    //////////////////////////////////////////////////////////////////////////////////
    //  bookkeeping
    //////////////////////////////////////////////////////////////////////////////////

    std::vector<double> CBaseCell::getBandWavelengths() const
    {
        assert(m_Material != nullptr);
        return m_Material->getBandWavelengths();
    }

    int CBaseCell::getBandIndex(double wavelength) const
    {
        return m_Material->getBandIndex(wavelength);
    }

    std::size_t CBaseCell::getBandSize() const
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

    //////////////////////////////////////////////////////////////////////////////////
    //  factories
    //////////////////////////////////////////////////////////////////////////////////

    CBaseCell makeSpecularCell(std::shared_ptr<CMaterial> material)
    {
        return CBaseCell(std::move(material), CSpecularCellDescription{}, CellKind::Specular);
    }

    CBaseCell makeUniformDiffuseCell(std::shared_ptr<CMaterial> material,
                                     CellDescription description)
    {
        return CBaseCell(std::move(material), std::move(description), CellKind::UniformDiffuse);
    }

    CBaseCell makeDirectionalDiffuseCell(std::shared_ptr<CMaterial> material,
                                         CellDescription description)
    {
        return CBaseCell(std::move(material), std::move(description), CellKind::DirectionalDiffuse);
    }

    CBaseCell makeHomogeneousDiffuseCell(std::shared_ptr<CMaterial> material,
                                         CellDescription description)
    {
        return CBaseCell(std::move(material), std::move(description), CellKind::HomogeneousDiffuse);
    }

    CBaseCell makeMaterialDirectionalDiffuseCell(std::shared_ptr<CMaterial> material,
                                                 CellDescription description)
    {
        return CBaseCell(
          std::move(material), std::move(description), CellKind::MaterialDirectionalDiffuse);
    }

    CBaseCell makeWovenCell(std::shared_ptr<CMaterial> material,
                            CWovenCellDescription description)
    {
        return CBaseCell(std::move(material), std::move(description), CellKind::Woven);
    }

    CBaseCell makeVenetianCell(std::shared_ptr<CMaterial> material,
                               CVenetianCellDescription description,
                               double rotation)
    {
        return CBaseCell(
          std::move(material), std::move(description), CellKind::Venetian, rotation);
    }
}   // namespace SingleLayerOptics
