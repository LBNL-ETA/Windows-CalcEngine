
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
    CDirectionalDiffuseBSDFLayer::CDirectionalDiffuseBSDFLayer(
      const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
      const CBSDFHemisphere & t_Hemisphere) :
        CBSDFLayer(t_Cell, t_Hemisphere)
    {}

    std::shared_ptr<CDirectionalDiffuseCell>
      CDirectionalDiffuseBSDFLayer::cellAsDirectionalDiffuse() const
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell =
          std::dynamic_pointer_cast<CDirectionalDiffuseCell>(m_Cell);
        assert(aCell != nullptr);
        return aCell;
    }

    void CDirectionalDiffuseBSDFLayer::calcDiffuseDistribution(
      const Side aSide,
      const CBeamDirection & incomingDirection,
      const size_t incomingDirectionIndex)
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell = cellAsDirectionalDiffuse();

        auto & tau = m_Results->getMatrix(aSide, PropertySimple::T);
        auto & Rho = m_Results->getMatrix(aSide, PropertySimple::R);

        const auto & jDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);

        size_t size = jDirections.size();

        for(size_t outgoingDirectionIndex = 0; outgoingDirectionIndex < size;
            ++outgoingDirectionIndex)
        {
            using ConstantsData::WCE_PI;

            const CBeamDirection jDirection = jDirections[outgoingDirectionIndex].centerPoint();

            const double aTau = aCell->T_dir_dif(aSide, incomingDirection, jDirection);
            const double aRho = aCell->R_dir_dif(aSide, incomingDirection, jDirection);

            tau(outgoingDirectionIndex, incomingDirectionIndex) += aTau / WCE_PI;
            Rho(outgoingDirectionIndex, incomingDirectionIndex) += aRho / WCE_PI;
        }
    }

    void CDirectionalDiffuseBSDFLayer::calcDiffuseDistribution_wv(
      const Side aSide,
      const CBeamDirection & incomingDirection,
      const size_t incomingDirectionIndex)
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell = cellAsDirectionalDiffuse();

        const CBSDFDirections oDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);

        size_t size = oDirections.size();

        for(size_t outgoingDirectionIndex = 0; outgoingDirectionIndex < size; ++outgoingDirectionIndex)
        {
            const CBeamDirection oDirection = oDirections[outgoingDirectionIndex].centerPoint();

            auto aTau = aCell->T_dir_dif_band(aSide, incomingDirection, oDirection);
            auto Ref = aCell->R_dir_dif_band(aSide, incomingDirection, oDirection);

            const size_t numWV = aTau.size();
            for(size_t j = 0; j < numWV; ++j)
            {
                using ConstantsData::WCE_PI;

                std::shared_ptr<CBSDFIntegrator> aResults = nullptr;
                aResults = (*m_WVResults)[j];
                assert(aResults != nullptr);
                auto & tau = aResults->getMatrix(aSide, PropertySimple::T);
                auto & rho = aResults->getMatrix(aSide, PropertySimple::R);
                tau(outgoingDirectionIndex, incomingDirectionIndex) += aTau[j] / WCE_PI;
                rho(outgoingDirectionIndex, incomingDirectionIndex) += Ref[j] / WCE_PI;
            }
        }
    }

}   // namespace SingleLayerOptics
