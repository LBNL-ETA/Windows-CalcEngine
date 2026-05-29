#include <stdexcept>
#include <utility>

#include "BSDFLayerMaker.hpp"
#include "BaseCell.hpp"
#include "UniformDiffuseBSDFLayer.hpp"
#include "DirectionalDiffuseBSDFLayer.hpp"
#include "CellDescription.hpp"
#include "SpecularCellDescription.hpp"
#include "SpecularBSDFLayer.hpp"
#include "VenetianCellDescription.hpp"
#include "PerforatedCellDescription.hpp"
#include "WovenCellDescription.hpp"
#include "FlatCellDescription.hpp"
#include "PhotovoltaicSpecularBSDFLayer.hpp"

namespace SingleLayerOptics
{
    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getSpecularLayer(const std::shared_ptr<CMaterial> & t_Material,
                                        const BSDFHemisphere & t_BSDF)
    {
        auto aCell = std::make_shared<CBaseCell>(makeSpecularCell(t_Material));
        return std::make_shared<CSpecularBSDFLayer>(aCell, t_BSDF);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getDirDifLayer(const std::shared_ptr<CMaterial> & t_Material,
                                      const BSDFHemisphere & t_BSDF)
    {
        auto aCell = std::make_shared<CBaseCell>(
          makeMaterialDirectionalDiffuseCell(t_Material, CSpecularCellDescription{}));
        return std::make_shared<CMaterialDirectionalDiffuseBSDFLayer>(aCell, t_BSDF);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getPhotovoltaicSpecularLayer(const std::shared_ptr<CMaterial> & t_Material,
                                                    const BSDFHemisphere & t_BSDF,
                                                    PVPowerPropertiesTable powerTable)
    {
        auto aCell = std::make_shared<CBaseCell>(makeSpecularCell(t_Material));
        auto aLayer = std::make_shared<PhotovoltaicSpecularBSDFLayer>(aCell, t_BSDF);
        aLayer->assignPowerTable(std::move(powerTable));
        return aLayer;
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getCircularPerforatedLayer(const std::shared_ptr<CMaterial> & t_Material,
                                                  const BSDFHemisphere & t_BSDF,
                                                  double x,
                                                  double y,
                                                  double thickness,
                                                  double radius)
    {
        auto aCell = std::make_shared<CBaseCell>(
          makeUniformDiffuseCell(t_Material, CCircularCellDescription{x, y, thickness, radius}));
        return std::make_shared<CUniformDiffuseBSDFLayer>(aCell, t_BSDF);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getRectangularPerforatedLayer(const std::shared_ptr<CMaterial> & t_Material,
                                                     const BSDFHemisphere & t_BSDF,
                                                     double x,
                                                     double y,
                                                     double thickness,
                                                     double xHole,
                                                     double yHole)
    {
        auto aCell = std::make_shared<CBaseCell>(makeUniformDiffuseCell(
          t_Material, CRectangularCellDescription{x, y, thickness, xHole, yHole}));
        return std::make_shared<CUniformDiffuseBSDFLayer>(aCell, t_BSDF);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getVenetianLayer(const std::shared_ptr<CMaterial> & t_Material,
                                        const BSDFHemisphere & t_BSDF,
                                        double slatWidth,
                                        double slatSpacing,
                                        double slatTiltAngle,
                                        double curvatureRadius,
                                        size_t numOfSlatSegments,
                                        DistributionMethod method,
                                        const bool isHorizontal)
    {
        FenestrationCommon::Venetian::Geometry geometry{
          slatWidth, slatSpacing, slatTiltAngle, curvatureRadius};
        return getVenetianLayer(
          t_Material, t_BSDF, geometry, numOfSlatSegments, method, isHorizontal);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getVenetianLayer(const std::shared_ptr<CMaterial> & t_Material,
                                        const BSDFHemisphere & t_BSDF,
                                        const FenestrationCommon::Venetian::Geometry & geometry,
                                        size_t numOfSlatSegments,
                                        DistributionMethod method,
                                        bool isHorizontal)
    {
        CVenetianCellDescription aCellDescription{geometry, numOfSlatSegments};

        const auto profileAnglesIncoming = t_BSDF.profileAngles(BSDFDirection::Incoming);
        aCellDescription.preCalculateForProfileAngles(FenestrationCommon::Side::Front,
                                                      profileAnglesIncoming);

        const auto profileAnglesOutgoing = t_BSDF.profileAngles(BSDFDirection::Outgoing);
        aCellDescription.preCalculateForProfileAngles(FenestrationCommon::Side::Back,
                                                      profileAnglesOutgoing);

        static const auto horizontalVenetianRotation{0.0};
        static const auto verticalVenetianRotation{90.0};
        const auto rotation{isHorizontal ? horizontalVenetianRotation : verticalVenetianRotation};

        // Match pre-Phase-A behaviour: only the UniformDiffuse path applies the horizontal/vertical
        // rotation. The DirectionalDiffuse path ignores it (the directional cell pre-dates the
        // rotation feature and the existing tests have been validated without it).
        const auto cellRotation =
          (method == DistributionMethod::UniformDiffuse) ? rotation : 0.0;

        auto aCell = std::make_shared<CBaseCell>(
          makeVenetianCell(t_Material, std::move(aCellDescription), cellRotation));

        if(method == DistributionMethod::UniformDiffuse)
        {
            return std::make_shared<CUniformDiffuseBSDFLayer>(aCell, t_BSDF);
        }
        return std::make_shared<CDirectionalDiffuseBSDFLayer>(aCell, t_BSDF);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getPerfectlyDiffuseLayer(const std::shared_ptr<CMaterial> & t_Material,
                                                const BSDFHemisphere & t_BSDF)
    {
        auto aCell = std::make_shared<CBaseCell>(
          makeUniformDiffuseCell(t_Material, CFlatCellDescription{}));
        return std::make_shared<CUniformDiffuseBSDFLayer>(aCell, t_BSDF);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getHomogeneousDiffuseLayer(const std::shared_ptr<CMaterial> & t_Material,
                                                  const BSDFHemisphere & t_BSDF)
    {
        auto aCell = std::make_shared<CBaseCell>(
          makeHomogeneousDiffuseCell(t_Material, CFlatCellDescription{}));
        return std::make_shared<CHomogeneousDiffuseBSDFLayer>(aCell, t_BSDF);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getDirectionalDiffuseLayer(const std::shared_ptr<CMaterial> & t_Material,
                                                  const BSDFHemisphere & t_BSDF)
    {
        auto aCell = std::make_shared<CBaseCell>(
          makeDirectionalDiffuseCell(t_Material, CFlatCellDescription{}));
        return std::make_shared<CDirectionalDiffuseBSDFLayer>(aCell, t_BSDF);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getPreLoadedBSDFLayer(const std::shared_ptr<CMaterial> & t_Material,
                                             const BSDFHemisphere & t_BSDF)
    {
        auto aCell = std::make_shared<CBaseCell>(
          makeDirectionalDiffuseCell(t_Material, CFlatCellDescription{}));
        return std::make_shared<CMatrixBSDFLayer>(aCell, t_BSDF);
    }

    std::shared_ptr<CBSDFLayer>
      CBSDFLayerMaker::getWovenLayer(const std::shared_ptr<CMaterial> & t_Material,
                                     const BSDFHemisphere & t_BSDF,
                                     double diameter,
                                     double spacing)
    {
        auto aCell = std::make_shared<CBaseCell>(
          makeWovenCell(t_Material, CWovenCellDescription{diameter, spacing}));
        return std::make_shared<CUniformDiffuseBSDFLayer>(aCell, t_BSDF);
    }

    CBSDFLayerMaker::CBSDFLayerMaker(const std::shared_ptr<CMaterial> & t_Material,
                                     const BSDFHemisphere & t_BSDF,
                                     std::optional<CellDescription> t_Description,
                                     DistributionMethod t_Method) :
        m_Cell(nullptr)
    {
        if(t_Material == nullptr)
        {
            throw std::runtime_error("Material for BSDF layer must be defined.");
        }

        // Specular BSDF layer is considered to be default. Default is used if no description is
        // provided.
        const CellDescription & description =
          t_Description.value_or(CellDescription{CSpecularCellDescription{}});

        if(std::holds_alternative<CSpecularCellDescription>(description))
        {
            m_Layer = getSpecularLayer(t_Material, t_BSDF);
        }
        else if(std::holds_alternative<CFlatCellDescription>(description))
        {
            m_Layer = getPerfectlyDiffuseLayer(t_Material, t_BSDF);
        }
        else if(const auto * venetian = std::get_if<CVenetianCellDescription>(&description))
        {
            m_Layer = getVenetianLayer(t_Material,
                                       t_BSDF,
                                       venetian->getVenetianGeometry(),
                                       venetian->numberOfSegments(),
                                       t_Method,
                                       0);
        }
        else if(const auto * circular = std::get_if<CCircularCellDescription>(&description))
        {
            m_Layer = getCircularPerforatedLayer(t_Material,
                                                 t_BSDF,
                                                 circular->xDimension(),
                                                 circular->yDimension(),
                                                 circular->thickness(),
                                                 circular->radius());
        }
        else if(const auto * rectangular = std::get_if<CRectangularCellDescription>(&description))
        {
            m_Layer = getRectangularPerforatedLayer(t_Material,
                                                    t_BSDF,
                                                    rectangular->xDimension(),
                                                    rectangular->yDimension(),
                                                    rectangular->thickness(),
                                                    rectangular->xHole(),
                                                    rectangular->yHole());
        }
        else if(const auto * woven = std::get_if<CWovenCellDescription>(&description))
        {
            // Woven shades do not work with directional diffuse algorithm
            m_Layer = getWovenLayer(t_Material, t_BSDF, woven->diameter(), woven->spacing());
        }
    }

    std::shared_ptr<CBSDFLayer> CBSDFLayerMaker::getLayer() const
    {
        return m_Layer;
    }

    std::shared_ptr<CBaseCell> CBSDFLayerMaker::getCell() const
    {
        return m_Cell;
    }
}   // namespace SingleLayerOptics
