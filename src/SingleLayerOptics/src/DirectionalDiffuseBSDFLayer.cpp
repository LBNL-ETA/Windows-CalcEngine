
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
		const std::shared_ptr< CDirectionalDiffuseCell > & t_Cell,
		const CBSDFHemisphere & t_Hemisphere ) :
        CBSDFLayer(t_Cell, t_Hemisphere)
    {}

    std::shared_ptr<CDirectionalDiffuseCell> CDirectionalDiffuseBSDFLayer::cellAsDirectionalDiffuse() const
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell = std::dynamic_pointer_cast<CDirectionalDiffuseCell>(m_Cell);
        assert(aCell != nullptr);
        return aCell;
    }

    void CDirectionalDiffuseBSDFLayer::calcDiffuseDistribution(const Side aSide, const CBeamDirection & t_Direction, const size_t t_DirectionIndex)
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell = cellAsDirectionalDiffuse();

        auto & tau = m_Results->getMatrix(aSide, PropertySimple::T);
        auto & Rho = m_Results->getMatrix(aSide, PropertySimple::R);

        const auto & jDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);

        size_t size = jDirections.size();

        for(size_t j = 0; j < size; ++j)
        {
            using ConstantsData::WCE_PI;

            const CBeamDirection jDirection = jDirections[j].centerPoint();

            double aTau = aCell->T_dir_dir(aSide, t_Direction, jDirection);
            double aRho = aCell->R_dir_dir(aSide, t_Direction, jDirection);

            tau(j, t_DirectionIndex) += aTau / WCE_PI;
            Rho(j, t_DirectionIndex) += aRho / WCE_PI;
        }
    }

    void CDirectionalDiffuseBSDFLayer::calcDiffuseDistribution_wv(const Side aSide, const CBeamDirection & t_Direction, const size_t t_DirectionIndex)
    {
        std::shared_ptr<CDirectionalDiffuseCell> aCell = cellAsDirectionalDiffuse();

        const CBSDFDirections oDirections = m_BSDFHemisphere.getDirections(BSDFDirection::Outgoing);

        size_t size = oDirections.size();

        for(size_t i = 0; i < size; ++i)
        {
            const CBeamDirection oDirection = oDirections[i].centerPoint();

            std::shared_ptr<std::vector<double>> aTau = aCell->T_dir_dir_band(aSide, t_Direction, oDirection);
            std::shared_ptr<std::vector<double>> Ref = aCell->R_dir_dir_band(aSide, t_Direction, oDirection);

            size_t numWV = aTau->size();
            for(size_t j = 0; j < numWV; ++j)
            {
                using ConstantsData::WCE_PI;

                std::shared_ptr<CBSDFIntegrator> aResults = nullptr;
                aResults = (*m_WVResults)[j];
                assert(aResults != nullptr);
                auto & tau = aResults->getMatrix(aSide, PropertySimple::T);
                auto & rho = aResults->getMatrix(aSide, PropertySimple::R);
                tau(t_DirectionIndex, i) += (*aTau)[j] / WCE_PI;
                rho(t_DirectionIndex, i) += (*Ref)[j] / WCE_PI;
            }
        }
    }

}   // namespace SingleLayerOptics
