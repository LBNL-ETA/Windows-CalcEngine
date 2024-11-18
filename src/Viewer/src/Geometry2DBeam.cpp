#include <cassert>
#include <algorithm>
#include <stdexcept>

#include "Geometry2DBeam.hpp"
#include "Geometry2D.hpp"
#include "ViewSegment2D.hpp"
#include "Segment2D.hpp"
#include "Point2D.hpp"
#include "ViewerConstants.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

namespace Viewer
{
    ////////////////////////////////////////////////////////////////////////////////////////
    // BeamViewFactor
    ////////////////////////////////////////////////////////////////////////////////////////
    BeamViewFactor::BeamViewFactor(size_t const t_Geometry2DIndex,
                                   size_t const t_SegmentIndex,
                                   double const t_Value,
                                   double const t_PercentHit) :
        enclosureIndex(t_Geometry2DIndex),
        segmentIndex(t_SegmentIndex),
        value(t_Value),
        percentHit(t_PercentHit)
    {}

    bool BeamViewFactor::operator==(BeamViewFactor const & t_BVF) const
    {
        return (t_BVF.enclosureIndex == this->enclosureIndex)
               && (t_BVF.segmentIndex == this->segmentIndex);
    }


    ////////////////////////////////////////////////////////////////////////////////////////
    // CDirect2DBeam
    ////////////////////////////////////////////////////////////////////////////////////////
    CDirect2DBeam::CDirect2DBeam(const CViewSegment2D & t_Beam) : m_Beam(t_Beam)
    {}

    // Check if segment intersects with the beam
    void CDirect2DBeam::checkSegment(const CViewSegment2D & t_Segment)
    {
        auto aStatus = m_Beam.intersectionWithLine(t_Segment);
        if(aStatus != IntersectionStatus::No)
        {
            m_Segments.push_back(t_Segment);
        }
    }

    double CDirect2DBeam::Side() const
    {
        return m_Beam.startPoint().y();
    }

    std::optional<CViewSegment2D>
      CDirect2DBeam::getClosestCommonSegment(const CDirect2DBeam & t_Beam,
                                             const BeamPosition beamPosition) const
    {
        std::optional<CViewSegment2D> aSegment;

        // Define the comparison function based on the beam position
        auto compare = [beamPosition](double x1, double x2) {
            return beamPosition == BeamPosition::Outside ? x1 > x2 : x1 < x2;
        };

        for(const auto & thisSegment : m_Segments)
        {
            if(t_Beam.isSegmentIn(thisSegment))
            {
                if(!aSegment.has_value()
                   || compare(aSegment->centerPoint().x(), thisSegment.centerPoint().x()))
                {
                    aSegment = thisSegment;
                }
            }
        }

        return aSegment;
    }


    double CDirect2DBeam::cosAngle(const CViewSegment2D & t_Segment) const
    {
        return m_Beam.dotProduct(t_Segment) / m_Beam.length();
    }

    bool CDirect2DBeam::isSegmentIn(const CViewSegment2D & t_Segment) const
    {
        auto isIn = false;
        for(const auto & thisSegment : m_Segments)
        {
            if(thisSegment == t_Segment)
            {
                isIn = true;
                break;
            }
        }
        return isIn;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // CDirect2DRay
    ////////////////////////////////////////////////////////////////////////////////////////

    CDirect2DRay::CDirect2DRay(const CDirect2DBeam & t_Beam1, const CDirect2DBeam & t_Beam2) :
        m_Beam1(t_Beam1), m_Beam2(t_Beam2)
    {}

    CDirect2DRay::CDirect2DRay(const CViewSegment2D & t_Ray1, const CViewSegment2D & t_Ray2) :
        m_Beam1(t_Ray1), m_Beam2(t_Ray2)
    {}

    double CDirect2DRay::rayNormalHeight() const
    {
        return m_Beam1.Side() - m_Beam2.Side();
    }

    void CDirect2DRay::checkSegment(const CViewSegment2D & t_Segment)
    {
        m_Beam1.checkSegment(t_Segment);
        m_Beam2.checkSegment(t_Segment);
    }

    // Return segment hit by the ray
    std::optional<CViewSegment2D>
      CDirect2DRay::closestSegmentHit(const BeamPosition beamPosition) const
    {
        return m_Beam1.getClosestCommonSegment(m_Beam2, beamPosition);
    }

    double CDirect2DRay::cosAngle(const CViewSegment2D & t_Segment) const
    {
        return m_Beam1.cosAngle(t_Segment);
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // CDirect2DRayResult
    ////////////////////////////////////////////////////////////////////////////////////////

    CDirect2DRaysResult::CDirect2DRaysResult(double const t_ProfileAngle,
                                             double const t_DirectToDirect,
                                             std::vector<BeamViewFactor> t_BeamViewFactors) :
        m_ViewFactors(std::move(t_BeamViewFactors)),
        m_DirectToDirect(t_DirectToDirect),
        m_ProfileAngle(t_ProfileAngle)
    {}

    std::vector<BeamViewFactor> CDirect2DRaysResult::beamViewFactors() const
    {
        return m_ViewFactors;
    }

    double CDirect2DRaysResult::directToDirect() const
    {
        return m_DirectToDirect;
    }

    double CDirect2DRaysResult::profileAngle() const
    {
        return m_ProfileAngle;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // CDirect2DRays
    ////////////////////////////////////////////////////////////////////////////////////////

    CDirect2DRays::CDirect2DRays(Side const t_Side) : m_Side(t_Side)
    {}

    void CDirect2DRays::appendGeometry2D(const CGeometry2D & t_Geometry2D)
    {
        m_Geometries2D.push_back(t_Geometry2D);
    }

    std::vector<BeamViewFactor> CDirect2DRays::beamViewFactors(double const t_ProfileAngle,
                                                               const BeamPosition beamPosition)
    {
        checkForProfileAngle(t_ProfileAngle, beamPosition);
        return m_RayResults.at(keyFromProfileAngle(t_ProfileAngle, beamPosition)).beamViewFactors();
    }

    double CDirect2DRays::directToDirect(double const t_ProfileAngle,
                                         const BeamPosition beamPosition)
    {
        checkForProfileAngle(t_ProfileAngle, beamPosition);
        return m_RayResults.at(keyFromProfileAngle(t_ProfileAngle, beamPosition)).directToDirect();
    }

    void CDirect2DRays::checkForProfileAngle(const double t_ProfileAngle,
                                             const BeamPosition beamPosition)
    {   // Need to have this in case the profile angle is not precalculated
        if(!m_RayResults.count(keyFromProfileAngle(t_ProfileAngle, beamPosition)))
        {
            m_RayResults[keyFromProfileAngle(t_ProfileAngle, beamPosition)] =
              calculateAllProperties(t_ProfileAngle, beamPosition);
        }
    }

    CDirect2DRaysResult CDirect2DRays::calculateAllProperties(double const t_ProfileAngle,
                                                              const BeamPosition beamPosition)
    {
        auto boundaries{findRayBoundaries(t_ProfileAngle)};
        auto rays{findInBetweenRays(t_ProfileAngle, boundaries)};
        return calculateBeamProperties(t_ProfileAngle, beamPosition, rays);
    }

    CDirect2DRays::RayBoundaries CDirect2DRays::findRayBoundaries(double const t_ProfileAngle)
    {
        RayBoundaries result;

        for(size_t i = 0; i < m_Geometries2D.size(); ++i)
        {
            const auto & aGeometry{m_Geometries2D[i]};
            const CPoint2D aPoint{m_Side == Side::Front ? aGeometry.entryPoint()
                                                        : aGeometry.exitPoint()};

            CViewSegment2D entryRay{createSubBeam(aPoint, t_ProfileAngle)};
            if(i == 0u)
            {
                result.m_LowerRay = entryRay;
                result.m_UpperRay = entryRay;
            }
            else
            {
                // This sets profile angle for point comparison that follows in next lines
                auto aProfilePoint = PointsProfile2DCompare(t_ProfileAngle);
                if(aProfilePoint(result.m_LowerRay.startPoint(), entryRay.startPoint()))
                {
                    result.m_LowerRay = entryRay;
                }
                if(!aProfilePoint(result.m_UpperRay.startPoint(), entryRay.startPoint()))
                {
                    result.m_UpperRay = entryRay;
                }
            }
        }

        return result;
    }

    std::vector<CDirect2DRay> CDirect2DRays::findInBetweenRays(double const t_ProfileAngle,
                                                               RayBoundaries & boudnaries)
    {
        std::vector<CPoint2D> inBetweenPoints;

        for(const auto & aEnclosure : m_Geometries2D)
        {
            auto aSegments = aEnclosure.segments();
            if(boudnaries.isInRay(aSegments[0].startPoint()))
            {
                inBetweenPoints.push_back(aSegments[0].startPoint());
            }
            for(const auto & aSegment : aSegments)
            {
                auto endPoint = aSegment.endPoint();
                // Ray is always going from left to right. For point to be in between beam, it must
                // be visible for upper ray and invisible for lower ray
                if(boudnaries.m_UpperRay.position(endPoint) == PointPosition::Visible
                   && boudnaries.m_LowerRay.position(endPoint) == PointPosition::Invisible)
                {
                    inBetweenPoints.push_back(endPoint);
                }
            }
        }

        std::vector<CDirect2DRay> rays;

        sort(
          inBetweenPoints.begin(), inBetweenPoints.end(), PointsProfile2DCompare(t_ProfileAngle));

        // Creating incoming rays
        CViewSegment2D firstBeam = boudnaries.m_UpperRay;
        for(auto aPoint : inBetweenPoints)
        {
            CViewSegment2D secondBeam{createSubBeam(aPoint, t_ProfileAngle)};
            CDirect2DRay aRay(firstBeam, secondBeam);

            // Don't save rays that are smaller than distance tolerance
            if(aRay.rayNormalHeight() > ViewerConstants::DISTANCE_TOLERANCE)
            {
                rays.emplace_back(firstBeam, secondBeam);
            }
            firstBeam = secondBeam;
        }
        rays.emplace_back(firstBeam, boudnaries.m_LowerRay);

        return rays;
    }

    CDirect2DRaysResult CDirect2DRays::calculateBeamProperties(double const t_ProfileAngle,
                                                               const BeamPosition beamPosition,
                                                               std::vector<CDirect2DRay> & rays)
    {
        // First check all segments and calculate total ray height
        auto totalHeight = 0.0;
        for(auto & beamRay : rays)
        {
            totalHeight += beamRay.rayNormalHeight();
            for(const auto & aEnclosure : m_Geometries2D)
            {
                for(const auto & aSegment : aEnclosure.segments())
                {
                    beamRay.checkSegment(aSegment);
                }
            }
        }

        // Now calculate beam view factors
        std::vector<BeamViewFactor> aViewFactors;
        double aDirectToDirect = 0;
        // Create beam direction parallel to x-axe
        CPoint2D sPoint(0, 0);
        CPoint2D ePoint(1, 0);
        CViewSegment2D aNormalBeamDirection{sPoint, ePoint};
        for(const auto & beamRay : rays)
        {
            auto currentHeight = beamRay.rayNormalHeight();
            auto projectedBeamHeight = beamRay.cosAngle(aNormalBeamDirection);
            auto viewFactor = 0.0;
            auto percentHit = 0.0;
            auto closestSegment = beamRay.closestSegmentHit(beamPosition);
            for(size_t e = 0; e < m_Geometries2D.size(); ++e)
            {
                for(size_t s = 0; s < m_Geometries2D[e].segments().size(); ++s)
                {
                    auto currentSegment = m_Geometries2D[e].segments()[s];
                    if(currentSegment == beamRay.closestSegmentHit(beamPosition))
                    {
                        viewFactor = currentHeight / totalHeight;
                        projectedBeamHeight = projectedBeamHeight * currentHeight;
                        auto segmentHitLength =
                          projectedBeamHeight
                          / std::abs(beamRay.cosAngle(currentSegment.getNormal()));
                        percentHit = segmentHitLength / currentSegment.length();
                        auto aTest = find(
                          aViewFactors.begin(), aViewFactors.end(), BeamViewFactor(e, s, 0, 0));
                        if(aTest != aViewFactors.end())
                        {
                            auto & aVF = *aTest;
                            aVF.value += viewFactor;
                            aVF.percentHit += percentHit;
                        }
                        else
                        {
                            auto aVF = BeamViewFactor(e, s, viewFactor, percentHit);
                            aViewFactors.push_back(aVF);
                        }
                    }
                }
            }

            // No segment is being hit. That means ray goes through. Add this to total view factor
            if(viewFactor == 0)
            {
                aDirectToDirect += currentHeight / totalHeight;
            }
        }
        return {t_ProfileAngle, aDirectToDirect, aViewFactors};
    }

    bool CDirect2DRays::RayBoundaries::isInRay(CPoint2D const & t_Point) const
    {
        return m_UpperRay.position(t_Point) == PointPosition::Visible
               && m_LowerRay.position(t_Point) == PointPosition::Invisible;
    }

    CViewSegment2D CDirect2DRays::createSubBeam(CPoint2D const & t_Point,
                                                double const t_ProfileAngle) const
    {
        auto const deltaX = 100.0;
        auto const tanPhi = std::tan(radians(t_ProfileAngle));
        auto yStart = t_Point.y() - t_Point.x() * tanPhi;
        auto yEnd = yStart + deltaX * tanPhi;

        CPoint2D startPoint(0, yStart);
        CPoint2D endPoint(deltaX, yEnd);
        return {startPoint, endPoint};
    }

    void CDirect2DRays::precalculateForProfileAngles(const std::vector<double> & t_ProfileAngles,
                                                     const BeamPosition beamPosition)
    {
        for(const auto & angle : t_ProfileAngles)
        {
            m_RayResults[keyFromProfileAngle(angle, beamPosition)] =
              calculateAllProperties(angle, beamPosition);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // CGeometry2DBeam
    ////////////////////////////////////////////////////////////////////////////////////////

    CGeometry2DBeam::CGeometry2DBeam() :
        m_Ray{{Side::Front, CDirect2DRays(Side::Front)}, {Side::Back, CDirect2DRays(Side::Back)}}
    {}

    void CGeometry2DBeam::appendGeometry2D(const CGeometry2D & t_Geometry2D)
    {
        for(auto & [key, ray] : m_Ray)
        {
            std::ignore = key;
            ray.appendGeometry2D(t_Geometry2D);
        }
    }

    // Returns non zero view factors. It also calculates direct to direct component for the beam
    std::vector<BeamViewFactor> CGeometry2DBeam::beamViewFactors(double const t_ProfileAngle,
                                                                 Side const t_Side)
    {
        const BeamPosition beamPosition =
          t_Side == Side::Front ? BeamPosition::Outside : BeamPosition::Inside;
        return m_Ray.at(t_Side).beamViewFactors(t_ProfileAngle, beamPosition);
    }

    double CGeometry2DBeam::directToDirect(double const t_ProfileAngle, Side const t_Side)
    {
        const BeamPosition beamPosition =
          t_Side == Side::Front ? BeamPosition::Outside : BeamPosition::Inside;
        return m_Ray.at(t_Side).directToDirect(t_ProfileAngle, beamPosition);
    }

    void CGeometry2DBeam::precalculateForProfileAngles(FenestrationCommon::Side side,
                                                       const std::vector<double> & t_ProfileAngles)
    {
        const BeamPosition beamPosition =
          side == Side::Front ? BeamPosition::Outside : BeamPosition::Inside;
        m_Ray.at(side).precalculateForProfileAngles(t_ProfileAngles, beamPosition);
    }

    long long int keyFromProfileAngle(const double angle, const BeamPosition position)
    {
        constexpr auto precision{1e9};
        const auto angleKey = static_cast<long long>(angle * precision);

        // Assign a unique multiplier for each direction
        int positionKey = (position == BeamPosition::Inside) ? 1 : 2;

        // Shift the angleKey to make space for positionKey and combine
        return angleKey * 10 + positionKey;
    }

}   // namespace Viewer
