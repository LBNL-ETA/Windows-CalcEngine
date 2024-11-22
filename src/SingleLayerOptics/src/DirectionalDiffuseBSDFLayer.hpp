#pragma once

#include <memory>

#include "BSDFLayer.hpp"

namespace SingleLayerOptics
{
    class CDirectionalDiffuseCell;

    // All outgoing directions are calculated
    class CDirectionalBSDFLayer : public CBSDFLayer
    {
    public:
        CDirectionalBSDFLayer(const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
                              const BSDFHemisphere & t_Hemisphere);

    protected:
        std::shared_ptr<CDirectionalDiffuseCell> cellAsDirectionalDiffuse() const;
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

        std::vector<double> lambdas;

        virtual double diffuseDistributionScalar(size_t incomingDirection, size_t outgoingDirection) = 0;
    };

    class CDirectionalDiffuseBSDFLayer : public CDirectionalBSDFLayer
    {
    public:
        CDirectionalDiffuseBSDFLayer(const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
                                     const BSDFHemisphere & t_Hemisphere);

    protected:
        double diffuseDistributionScalar(size_t incomingDirection, size_t outgoingDirection) override;
    };

    class CHomogeneousDiffuseBSDFLayer : public CDirectionalBSDFLayer
    {
    public:
        CHomogeneousDiffuseBSDFLayer(const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
                                     const BSDFHemisphere & t_Hemisphere);

    protected:
        double diffuseDistributionScalar(size_t incomingDirection, size_t outgoingDirection) override;
    };

    class CMatrixBSDFLayer : public CDirectionalBSDFLayer
    {
    public:
        CMatrixBSDFLayer(const std::shared_ptr<CDirectionalDiffuseCell> & t_Cell,
                         const BSDFHemisphere & t_Hemisphere);

    protected:
        double diffuseDistributionScalar(size_t incomingDirection, size_t outgoingDirection) override;
    };

}   // namespace SingleLayerOptics
