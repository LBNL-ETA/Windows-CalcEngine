
#include <cmath>
#include <cassert>

#include "UniformDiffuseBSDFLayer.hpp"
#include "UniformDiffuseCell.hpp"
#include "BSDFIntegrator.hpp"
#include "BSDFDirections.hpp"
#include "WCECommon.hpp"
#include "BeamDirection.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CUniformDiffuseBSDFLayer::CUniformDiffuseBSDFLayer(
      const std::shared_ptr<CUniformDiffuseCell> & t_Cell, const BSDFHemisphere & t_Hemisphere) :
        CBSDFLayer(t_Cell, t_Hemisphere)
    {}

    std::shared_ptr<CUniformDiffuseCell> CUniformDiffuseBSDFLayer::cellAsUniformDiffuse() const
    {
        std::shared_ptr<CUniformDiffuseCell> aCell =
          std::dynamic_pointer_cast<CUniformDiffuseCell>(m_Cell);
        assert(aCell != nullptr);
        return aCell;
    }

    void CUniformDiffuseBSDFLayer::calcDiffuseDistribution(const Side aSide,
                                                           const CBeamDirection & t_Direction,
                                                           const size_t t_DirectionIndex)
    {
        std::shared_ptr<CUniformDiffuseCell> aCell = cellAsUniformDiffuse();

        auto & Tau = m_Results.getMatrix(aSide, PropertySimple::T);
        auto & Rho = m_Results.getMatrix(aSide, PropertySimple::R);

        double aTau = aCell->T_dir_dif(aSide, t_Direction);
        double Ref = aCell->R_dir_dif(aSide, t_Direction);

        const BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
        size_t size = aDirections.size();

        for(size_t j = 0; j < size; ++j)
        {
            using ConstantsData::WCE_PI;

            Tau(j, t_DirectionIndex) += aTau / WCE_PI;
            Rho(j, t_DirectionIndex) += Ref / WCE_PI;
        }
    }

    void CUniformDiffuseBSDFLayer::calcDiffuseDistribution_wv(const Side aSide,
                                                              const CBeamDirection & t_Direction,
                                                              const size_t t_DirectionIndex)
    {
        std::shared_ptr<CUniformDiffuseCell> aCell = cellAsUniformDiffuse();

        std::vector<double> aTau = aCell->T_dir_dif_band(aSide, t_Direction);
        std::vector<double> Ref = aCell->R_dir_dif_band(aSide, t_Direction);

        const BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
        size_t size = aDirections.size();

        for(size_t i = 0; i < size; ++i)
        {
            size_t numWV = aTau.size();
            for(size_t j = 0; j < numWV; ++j)
            {
                using ConstantsData::WCE_PI;

                auto & tau = m_WVResults[j].getMatrix(aSide, PropertySimple::T);
                auto & rho = m_WVResults[j].getMatrix(aSide, PropertySimple::R);
                tau(i, t_DirectionIndex) += aTau[j] / WCE_PI;
                rho(i, t_DirectionIndex) += Ref[j] / WCE_PI;
            }
        }
    }

    void CUniformDiffuseBSDFLayer::calcDiffuseDistribution_byWavelength(
      const FenestrationCommon::Side aSide,
      const CBeamDirection & t_Direction,
      const size_t t_DirectionIndex,
      size_t wavelengthIndex,
      BSDFIntegrator & results)
    {
        std::shared_ptr<CUniformDiffuseCell> aCell = cellAsUniformDiffuse();

        const auto aTau = aCell->T_dir_dif_at_wavelength(aSide, t_Direction, wavelengthIndex);
        const auto Ref = aCell->R_dir_dif_at_wavelength(aSide, t_Direction, wavelengthIndex);

        const BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
        size_t size = aDirections.size();

        for(size_t i = 0; i < size; ++i)
        {
            using ConstantsData::WCE_PI;

            auto & tau = results.getMatrix(aSide, PropertySimple::T);
            auto & rho = results.getMatrix(aSide, PropertySimple::R);
            tau(i, t_DirectionIndex) += aTau / WCE_PI;
            rho(i, t_DirectionIndex) += Ref / WCE_PI;
        }
    }

}   // namespace SingleLayerOptics
