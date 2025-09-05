#include <cassert>
#include <ranges>

#include "DirectionalDiffuseBSDFLayer.hpp"
#include "DirectionalDiffuseCell.hpp"
#include "BSDFIntegrator.hpp"
#include "BSDFDirections.hpp"
#include "BeamDirection.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CDirectionalBSDFLayer::CDirectionalBSDFLayer(
      const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
      const BSDFHemisphere & t_Hemisphere,
      WeightFn && weightFn) :
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

    void CDirectionalBSDFLayer::calcDiffuseDistribution(const Side side,
                                                        const CBeamDirection & inDir,
                                                        const size_t inIdx)
    {
        auto * cell = cellAsDirectionalDiffuse();
        auto & tau = m_Results->getMatrix(side, PropertySurface::T);
        auto & rho = m_Results->getMatrix(side, PropertySurface::R);

        for_each_outgoing_([&](size_t out, const CBeamDirection & oDir, double s) {
            const double T = cell->T_dir_dif(side, inDir, oDir);
            const double R = cell->R_dir_dif(side, inDir, oDir);
            tau(out, inIdx) += T * s;
            rho(out, inIdx) += R * s;
        });
    }

    void CDirectionalBSDFLayer::calcDiffuseDistribution_wv(const Side side,
                                                           const CBeamDirection & inDir,
                                                           const size_t inIdx,
                                                           std::vector<BSDFIntegrator> & results)
    {
        auto * cell = cellAsDirectionalDiffuse();

        for_each_outgoing_([&](size_t out, const CBeamDirection & oDir, double s) {
            auto t = cell->T_dir_dif_band(side, inDir, oDir);
            auto r = cell->R_dir_dif_band(side, inDir, oDir);
            const size_t N = t.size();
            for(size_t j = 0; j < N; ++j)
            {
                auto & tau = results[j].getMatrix(side, PropertySurface::T);
                auto & rho = results[j].getMatrix(side, PropertySurface::R);
                tau(out, inIdx) += t[j] * s;
                rho(out, inIdx) += r[j] * s;
            }
        });
    }

    void CDirectionalBSDFLayer::calcDiffuseDistribution_byWavelength(const Side side,
                                                                     const CBeamDirection & inDir,
                                                                     const size_t inIdx,
                                                                     size_t wavelengthIndex,
                                                                     BSDFIntegrator & res)
    {
        auto * cell = cellAsDirectionalDiffuse();
        auto & tau = res.getMatrix(side, PropertySurface::T);
        auto & rho = res.getMatrix(side, PropertySurface::R);

        for_each_outgoing_([&](size_t out, const CBeamDirection & oDir, double s) {
            const double T = cell->T_dir_dif_by_wavelength(side, inDir, oDir, wavelengthIndex);
            const double R = cell->R_dir_dif_by_wavelength(side, inDir, oDir, wavelengthIndex);
            tau(out, inIdx) += T * s;
            rho(out, inIdx) += R * s;
        });
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
