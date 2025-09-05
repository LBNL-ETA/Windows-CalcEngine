#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <span>

#include "BSDFLayer.hpp"
#include "MaterialDirDifCell.hpp"

namespace SingleLayerOptics
{
    class CDirectionalDiffuseCell;

    // All outgoing directions are calculated
    class CDirectionalBSDFLayer : public CBSDFLayer
    {
    public:
        using WeightFn = std::function<double(double)>;

        CDirectionalBSDFLayer(
          const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
          const BSDFHemisphere & t_Hemisphere,
          WeightFn weightFn = [](double) { return 1.0; });

    protected:
        CDirectionalDiffuseCell * cellAsDirectionalDiffuse() const;

        void calcDiffuseDistribution(const FenestrationCommon::Side aSide,
                                     const CBeamDirection & incomingDirection,
                                     const size_t incomingDirectionIndex) override;
        void calcDiffuseDistribution_wv(const FenestrationCommon::Side aSide,
                                        const CBeamDirection & incomingDirection,
                                        const size_t incomingDirectionIndex,
                                        std::vector<BSDFIntegrator> & results) override;
        void calcDiffuseDistribution_byWavelength(FenestrationCommon::Side aSide,
                                                  const CBeamDirection & incomingDirection,
                                                  size_t incomingDirectionIndex,
                                                  size_t wavelengthIndex,
                                                  BSDFIntegrator & results) override;

        std::span<const double> weights() const noexcept
        {
            return {m_weights.data(), m_weights.size()};
        }

        std::vector<double> lambdas;

    private:
        std::vector<double> m_weights;
        WeightFn m_weightsFunction;
    };

    class CDirectionalDiffuseBSDFLayer : public CDirectionalBSDFLayer
    {
    public:
        CDirectionalDiffuseBSDFLayer(const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
                                     const BSDFHemisphere & t_Hemisphere);
    };

    class CHomogeneousDiffuseBSDFLayer : public CDirectionalBSDFLayer
    {
    public:
        CHomogeneousDiffuseBSDFLayer(const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
                                     const BSDFHemisphere & t_Hemisphere);
    };

    class CMaterialDirectionalDiffuseBSDFLayer : public CDirectionalBSDFLayer
    {
    public:
        CMaterialDirectionalDiffuseBSDFLayer(
          const std::shared_ptr<CMaterialDirectionalDiffuseCell> & t_Cell,
          const BSDFHemisphere & t_Hemisphere);
    };

    class CMatrixBSDFLayer : public CDirectionalBSDFLayer
    {
    public:
        CMatrixBSDFLayer(const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
                         const BSDFHemisphere & t_Hemisphere);
    };

}   // namespace SingleLayerOptics
