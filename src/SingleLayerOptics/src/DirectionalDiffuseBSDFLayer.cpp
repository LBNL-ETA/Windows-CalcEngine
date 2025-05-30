
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
      const BSDFHemisphere & t_Hemisphere) :
        CBSDFLayer(t_Cell, t_Hemisphere),
        lambdas(t_Hemisphere.getDirections(BSDFDirection::Outgoing).lambdaVector())
    {}

    std::shared_ptr<CDirectionalDiffuseCell> CDirectionalBSDFLayer::cellAsDirectionalDiffuse() const
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell =
          std::dynamic_pointer_cast<CDirectionalDiffuseCell>(m_Cell);
        assert(aCell != nullptr);
        return aCell;
    }

    void CDirectionalBSDFLayer::calcDiffuseDistribution(const Side aSide,
                                                        const CBeamDirection & incomingDirection,
                                                        const size_t incomingDirectionIndex)
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell = cellAsDirectionalDiffuse();

        auto & tau = m_Results.getMatrix(aSide, PropertySimple::T);
        auto & Rho = m_Results.getMatrix(aSide, PropertySimple::R);

        const auto & jDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);

        size_t size = jDirections.size();

        for(size_t outgoingDirectionIndex = 0; outgoingDirectionIndex < size;
            ++outgoingDirectionIndex)
        {
            const CBeamDirection jDirection = jDirections[outgoingDirectionIndex].centerPoint();

            const double aTau = aCell->T_dir_dif(aSide, incomingDirection, jDirection);
            const double aRho = aCell->R_dir_dif(aSide, incomingDirection, jDirection);

            tau(outgoingDirectionIndex, incomingDirectionIndex) +=
              aTau * diffuseDistributionScalar(incomingDirectionIndex, outgoingDirectionIndex);
            Rho(outgoingDirectionIndex, incomingDirectionIndex) +=
              aRho * diffuseDistributionScalar(incomingDirectionIndex, outgoingDirectionIndex);
        }
    }

    void CDirectionalBSDFLayer::calcDiffuseDistribution_wv(const Side aSide,
                                                           const CBeamDirection & incomingDirection,
                                                           const size_t incomingDirectionIndex,
                                                           std::vector<BSDFIntegrator> & results)
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell = cellAsDirectionalDiffuse();

        const BSDFDirections oDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);

        size_t size = oDirections.size();

        for(size_t outgoingDirectionIndex = 0; outgoingDirectionIndex < size;
            ++outgoingDirectionIndex)
        {
            const CBeamDirection oDirection = oDirections[outgoingDirectionIndex].centerPoint();

            auto aTau = aCell->T_dir_dif_band(aSide, incomingDirection, oDirection);
            auto Ref = aCell->R_dir_dif_band(aSide, incomingDirection, oDirection);

            const size_t numWV = aTau.size();
            for(size_t j = 0; j < numWV; ++j)
            {
                auto & tau = results[j].getMatrix(aSide, PropertySimple::T);
                auto & rho = results[j].getMatrix(aSide, PropertySimple::R);
                tau(outgoingDirectionIndex, incomingDirectionIndex) +=
                  aTau[j]
                  * diffuseDistributionScalar(incomingDirectionIndex, outgoingDirectionIndex);
                rho(outgoingDirectionIndex, incomingDirectionIndex) +=
                  Ref[j]
                  * diffuseDistributionScalar(incomingDirectionIndex, outgoingDirectionIndex);
            }
        }
    }

    void CDirectionalBSDFLayer::calcDiffuseDistribution_byWavelength(
      const FenestrationCommon::Side aSide,
      const CBeamDirection & incomingDirection,
      const size_t incomingDirectionIndex,
      size_t wavelengthIndex,
      BSDFIntegrator & results)
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell = cellAsDirectionalDiffuse();

        const BSDFDirections oDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);

        size_t size = oDirections.size();

        for(size_t outgoingDirectionIndex = 0; outgoingDirectionIndex < size;
            ++outgoingDirectionIndex)
        {
            const CBeamDirection oDirection = oDirections[outgoingDirectionIndex].centerPoint();

            auto aTau =
              aCell->T_dir_dif_by_wavelength(aSide, incomingDirection, oDirection, wavelengthIndex);
            auto Ref =
              aCell->R_dir_dif_by_wavelength(aSide, incomingDirection, oDirection, wavelengthIndex);

            auto & tau = results.getMatrix(aSide, PropertySimple::T);
            auto & rho = results.getMatrix(aSide, PropertySimple::R);

            tau(outgoingDirectionIndex, incomingDirectionIndex) +=
              aTau * diffuseDistributionScalar(incomingDirectionIndex, outgoingDirectionIndex);
            rho(outgoingDirectionIndex, incomingDirectionIndex) +=
              Ref * diffuseDistributionScalar(incomingDirectionIndex, outgoingDirectionIndex);
        }
    }

    CDirectionalDiffuseBSDFLayer::CDirectionalDiffuseBSDFLayer(
      const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
      const BSDFHemisphere & t_Hemisphere) :
        CDirectionalBSDFLayer(t_Cell, t_Hemisphere)
    {}

    double CDirectionalDiffuseBSDFLayer::diffuseDistributionScalar(size_t, size_t)
    {
        return 1;
    }

    CHomogeneousDiffuseBSDFLayer::CHomogeneousDiffuseBSDFLayer(
      const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
      const BSDFHemisphere & t_Hemisphere) :
        CDirectionalBSDFLayer(t_Cell, t_Hemisphere)
    {}

    double CHomogeneousDiffuseBSDFLayer::diffuseDistributionScalar(size_t incomingDirection, size_t)
    {
        return 1 / (FenestrationCommon::WCE_PI - lambdas.at(incomingDirection));
    }

    CMatrixBSDFLayer::CMatrixBSDFLayer(const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
                                       const BSDFHemisphere & t_Hemisphere) :
        CDirectionalBSDFLayer(t_Cell, t_Hemisphere)
    {}

    double CMatrixBSDFLayer::diffuseDistributionScalar(size_t, size_t outgoingDirection)
    {
        return 1 / lambdas.at(outgoingDirection);
    }
}   // namespace SingleLayerOptics
