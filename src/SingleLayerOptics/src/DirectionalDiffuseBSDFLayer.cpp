
#include <cmath>
#include <cassert>

#include "DirectionalDiffuseBSDFLayer.hpp"
#include "DirectionalDiffuseCell.hpp"
#include "BSDFIntegrator.hpp"
#include "BSDFDirections.hpp"
#include "BeamDirection.hpp"
#include "BSDFPatch.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CDirectionalBSDFLayer::CDirectionalBSDFLayer(
      const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
      const BSDFHemisphere & t_Hemisphere,
      WeightFn weightFn) :
        CBSDFLayer(t_Cell, t_Hemisphere),
        lambdas(t_Hemisphere.getDirections(BSDFDirection::Outgoing).lambdaVector()),
        m_weights(lambdas.size(), 1.0)
    {
        if(weightFn)
        {
            std::ranges::transform(lambdas, m_weights.begin(), weightFn);
        }
    }

    CDirectionalDiffuseCell * CDirectionalBSDFLayer::cellAsDirectionalDiffuse() const
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell =
          std::dynamic_pointer_cast<CDirectionalDiffuseCell>(m_Cell);
        assert(aCell != nullptr);
        return aCell.get();
    }

    void CDirectionalBSDFLayer::calcDiffuseDistribution(const Side aSide,
                                                        const CBeamDirection & incomingDirection,
                                                        const size_t incomingDirectionIndex)
    {
        auto aCell = cellAsDirectionalDiffuse();

        auto & tau = m_Results.getMatrix(aSide, PropertySurface::T);
        auto & Rho = m_Results.getMatrix(aSide, PropertySurface::R);

        const auto & jDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);

        const auto & weight = weights();

        size_t size = jDirections.size();

        for(size_t outgoingDirectionIndex = 0; outgoingDirectionIndex < size;
            ++outgoingDirectionIndex)
        {
            const CBeamDirection jDirection = jDirections[outgoingDirectionIndex].centerPoint();

            const double aTau = aCell->T_dir_dif(aSide, incomingDirection, jDirection);
            const double aRho = aCell->R_dir_dif(aSide, incomingDirection, jDirection);

            tau(outgoingDirectionIndex, incomingDirectionIndex) +=
              aTau * weight[outgoingDirectionIndex];
            Rho(outgoingDirectionIndex, incomingDirectionIndex) +=
              aRho * weight[outgoingDirectionIndex];
        }
    }

    void CDirectionalBSDFLayer::calcDiffuseDistribution_wv(const Side aSide,
                                                           const CBeamDirection & incomingDirection,
                                                           const size_t incomingDirectionIndex,
                                                           std::vector<BSDFIntegrator> & results)
    {
        auto aCell = cellAsDirectionalDiffuse();

        const auto & oDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);

        const auto & weight = weights();

        const size_t size = oDirections.size();

        for(size_t outgoingDirectionIndex = 0; outgoingDirectionIndex < size;
            ++outgoingDirectionIndex)
        {
            const CBeamDirection oDirection = oDirections[outgoingDirectionIndex].centerPoint();

            auto aTau = aCell->T_dir_dif_band(aSide, incomingDirection, oDirection);
            auto Ref = aCell->R_dir_dif_band(aSide, incomingDirection, oDirection);

            const size_t numWV = aTau.size();
            for(size_t j = 0; j < numWV; ++j)
            {
                auto & tau = results[j].getMatrix(aSide, PropertySurface::T);
                auto & rho = results[j].getMatrix(aSide, PropertySurface::R);
                tau(outgoingDirectionIndex, incomingDirectionIndex) +=
                  aTau[j] * weight[outgoingDirectionIndex];
                rho(outgoingDirectionIndex, incomingDirectionIndex) +=
                  Ref[j] * weight[outgoingDirectionIndex];
            }
        }
    }

    void CDirectionalBSDFLayer::calcDiffuseDistribution_byWavelength(
      const Side aSide,
      const CBeamDirection & incomingDirection,
      const size_t incomingDirectionIndex,
      size_t wavelengthIndex,
      BSDFIntegrator & results)
    {
        auto aCell = cellAsDirectionalDiffuse();

        const auto & oDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);
        const auto & weight = weights();

        size_t size = oDirections.size();

        for(size_t outgoingDirectionIndex = 0; outgoingDirectionIndex < size;
            ++outgoingDirectionIndex)
        {
            const CBeamDirection oDirection = oDirections[outgoingDirectionIndex].centerPoint();

            auto aTau =
              aCell->T_dir_dif_by_wavelength(aSide, incomingDirection, oDirection, wavelengthIndex);
            auto Ref =
              aCell->R_dir_dif_by_wavelength(aSide, incomingDirection, oDirection, wavelengthIndex);

            auto & tau = results.getMatrix(aSide, PropertySurface::T);
            auto & rho = results.getMatrix(aSide, PropertySurface::R);

            tau(outgoingDirectionIndex, incomingDirectionIndex) +=
              aTau * weight[outgoingDirectionIndex];
            rho(outgoingDirectionIndex, incomingDirectionIndex) +=
              Ref * weight[outgoingDirectionIndex];
        }
    }

    CDirectionalDiffuseBSDFLayer::CDirectionalDiffuseBSDFLayer(
      const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
      const BSDFHemisphere & t_Hemisphere) :
        CDirectionalBSDFLayer(t_Cell, t_Hemisphere)
    {}

    CHomogeneousDiffuseBSDFLayer::CHomogeneousDiffuseBSDFLayer(
      const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
      const BSDFHemisphere & t_Hemisphere) :
        CDirectionalBSDFLayer(t_Cell, t_Hemisphere, [](double lam) { return 1.0 / (WCE_PI - lam); })
    {}

    CMaterialDirectionalDiffuseBSDFLayer::CMaterialDirectionalDiffuseBSDFLayer(
      const std::shared_ptr<CMaterialDirectionalDiffuseCell> & t_Cell,
      const BSDFHemisphere & t_Hemisphere) :
        CDirectionalBSDFLayer(t_Cell, t_Hemisphere)
    {}

    CMatrixBSDFLayer::CMatrixBSDFLayer(const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
                                       const BSDFHemisphere & t_Hemisphere) :
        CDirectionalBSDFLayer(t_Cell, t_Hemisphere, [](double lam) { return 1.0 / lam; })
    {}
}   // namespace SingleLayerOptics
