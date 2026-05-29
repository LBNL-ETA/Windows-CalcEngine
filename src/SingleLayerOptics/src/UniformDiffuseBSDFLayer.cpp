#include <cmath>

#include "UniformDiffuseBSDFLayer.hpp"
#include "BaseCell.hpp"
#include "BSDFIntegrator.hpp"
#include "BSDFDirections.hpp"
#include "WCECommon.hpp"
#include "BeamDirection.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CUniformDiffuseBSDFLayer::CUniformDiffuseBSDFLayer(const std::shared_ptr<CBaseCell> & t_Cell,
                                                       const BSDFHemisphere & t_Hemisphere) :
        CBSDFLayer(t_Cell, t_Hemisphere)
    {}

    void CUniformDiffuseBSDFLayer::calcDiffuseDistribution(const Side aSide,
                                                           const CBeamDirection & t_Direction,
                                                           const size_t t_DirectionIndex)
    {
        auto & Tau = m_Results->getMatrix(aSide, PropertySurface::T);
        auto & Rho = m_Results->getMatrix(aSide, PropertySurface::R);

        const double aTau = m_Cell->T_dir_dif(aSide, t_Direction);
        const double Ref = m_Cell->R_dir_dif(aSide, t_Direction);

        const BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
        const size_t size = aDirections.size();

        for(size_t j = 0; j < size; ++j)
        {
            using ConstantsData::WCE_PI;

            Tau(j, t_DirectionIndex) += aTau / WCE_PI;
            Rho(j, t_DirectionIndex) += Ref / WCE_PI;
        }
    }

    void CUniformDiffuseBSDFLayer::calcDiffuseDistribution_wv(const Side aSide,
                                                              const CBeamDirection & t_Direction,
                                                              const size_t t_DirectionIndex,
                                                              std::vector<BSDFIntegrator> & results)
    {
        std::vector<double> aTau = m_Cell->T_dir_dif_band(aSide, t_Direction);
        std::vector<double> Ref = m_Cell->R_dir_dif_band(aSide, t_Direction);

        const BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
        const size_t size = aDirections.size();

        for(size_t i = 0; i < size; ++i)
        {
            const size_t numWV = aTau.size();
            for(size_t j = 0; j < numWV; ++j)
            {
                using ConstantsData::WCE_PI;

                auto & tau = results[j].getMatrix(aSide, PropertySurface::T);
                auto & rho = results[j].getMatrix(aSide, PropertySurface::R);
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
        const auto aTau = m_Cell->T_dir_dif_at_wavelength(aSide, t_Direction, wavelengthIndex);
        const auto Ref = m_Cell->R_dir_dif_at_wavelength(aSide, t_Direction, wavelengthIndex);

        const BSDFDirections aDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Incoming);
        const size_t size = aDirections.size();

        for(size_t i = 0; i < size; ++i)
        {
            using ConstantsData::WCE_PI;

            auto & tau = results.getMatrix(aSide, PropertySurface::T);
            auto & rho = results.getMatrix(aSide, PropertySurface::R);
            tau(i, t_DirectionIndex) += aTau / WCE_PI;
            rho(i, t_DirectionIndex) += Ref / WCE_PI;
        }
    }

}   // namespace SingleLayerOptics
