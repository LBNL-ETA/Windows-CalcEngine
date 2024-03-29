#include <cassert>

#include "SpecularBSDFLayer.hpp"
#include "SpecularCell.hpp"
#include "BSDFDirections.hpp"
#include "WCECommon.hpp"
#include "BeamDirection.hpp"

using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CSpecularBSDFLayer::CSpecularBSDFLayer(const std::shared_ptr<CSpecularCell> & t_Cell,
                                           const BSDFHemisphere & t_Hemisphere) :
        CBSDFLayer(t_Cell, t_Hemisphere)
    {}


    std::shared_ptr<CSpecularCell> CSpecularBSDFLayer::cellAsSpecular() const
    {
        std::shared_ptr<CSpecularCell> aCell = std::dynamic_pointer_cast<CSpecularCell>(m_Cell);
        assert(aCell != nullptr);
        return aCell;
    }

    void
      CSpecularBSDFLayer::calcDiffuseDistribution(const Side, const CBeamDirection &, const size_t)
    {
        // No diffuse calculations are necessary for specular layer.
    }

    void CSpecularBSDFLayer::calcDiffuseDistribution_wv(const Side,
                                                        const CBeamDirection &,
                                                        const size_t,
                                                        std::vector<BSDFIntegrator> &)
    {
        // No diffuse calculations are necessary for specular layer.
    }

    void CSpecularBSDFLayer::calcDiffuseDistribution_byWavelength(const FenestrationCommon::Side,
                                                                  const CBeamDirection &,
                                                                  const size_t,
                                                                  size_t,
                                                                  BSDFIntegrator &)
    {
        // No diffusion in specular layers
    }

}   // namespace SingleLayerOptics
