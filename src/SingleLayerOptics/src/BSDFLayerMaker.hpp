#ifndef BSDFLAYERMAKER_H
#define BSDFLAYERMAKER_H

#include <memory>

namespace SingleLayerOptics
{
    enum class DistributionMethod
    {
        UniformDiffuse,
        DirectionalDiffuse
    };

    class ICellDescription;
    class CMaterial;
    class CBSDFHemisphere;
    class CBSDFLayer;
    class CBaseCell;

    // Class to simplify interface for BSDF layer creation
    class CBSDFLayerMaker
    {
    public:
        CBSDFLayerMaker(const std::shared_ptr<CMaterial> & t_Material,
                        const std::shared_ptr<const CBSDFHemisphere> & t_BSDF,
                        std::shared_ptr<ICellDescription> t_Description = nullptr,
                        const DistributionMethod t_Method = DistributionMethod::UniformDiffuse);

        std::shared_ptr<CBSDFLayer> getLayer();

        std::shared_ptr<CBaseCell> getCell() const;

        static std::shared_ptr<CBSDFLayer>
          getSpecularLayer(const std::shared_ptr<CMaterial> & t_Material,
                           const std::shared_ptr<const CBSDFHemisphere> & t_BSDF);

        static std::shared_ptr<CBSDFLayer>
          getCircularPerforatedLayer(const std::shared_ptr<CMaterial> & t_Material,
                                     const std::shared_ptr<const CBSDFHemisphere> & t_BSDF,
                                     double x,
                                     double y,
                                     double thickness,
                                     double radius);

        static std::shared_ptr<CBSDFLayer>
          getRectangularPerforatedLayer(const std::shared_ptr<CMaterial> & t_Material,
                                        const std::shared_ptr<const CBSDFHemisphere> & t_BSDF,
                                        const double x,
                                        const double y,
                                        const double thickness,
                                        const double xHole,
                                        const double yHole);

        static std::shared_ptr<CBSDFLayer>
          getVenetianLayer(const std::shared_ptr<CMaterial> & t_Material,
						   const std::shared_ptr<const CBSDFHemisphere> & t_BSDF,
						   const double slatWidth,
						   const double slatSpacing,
						   const double slatTiltAngle,
						   const double curvatureRadius,
						   const size_t numOfSlatSegments,
						   const DistributionMethod method = DistributionMethod::DirectionalDiffuse);

    private:
        std::shared_ptr<CBSDFLayer> m_Layer;
        std::shared_ptr<CBaseCell> m_Cell;
    };

}   // namespace SingleLayerOptics

#endif
