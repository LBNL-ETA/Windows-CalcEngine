#pragma once

#include <memory>
#include <vector>
#include <map>
#include <optional>

#include "ViewSegment2D.hpp"

namespace FenestrationCommon
{
    enum class Side;
}

namespace Viewer
{
    class CGeometry2D;
    class CSegment2D;
    class CPoint2D;

    ////////////////////////////////////////////////////////////////////////////////////////
    // BeamViewFactor
    ////////////////////////////////////////////////////////////////////////////////////////
    struct BeamViewFactor
    {
        BeamViewFactor(size_t t_Geometry2DIndex,
                       size_t t_SegmentIndex,
                       double t_Value,
                       double t_PercentHit);
        bool operator==(BeamViewFactor const & t_BVF1) const;
        // static bool isEqual( const BeamViewFactor& t_VF1, const BeamViewFactor& t_VF2 );
        size_t enclosureIndex;
        size_t segmentIndex;
        double value;
        double percentHit;
    };

    ////////////////////////////////////////////////////////////////////////////////////////
    // CDirect2DBeam
    ////////////////////////////////////////////////////////////////////////////////////////

    // Keeps information about single beam and segments that are intersected with it
    class CDirect2DBeam
    {
    public:
        explicit CDirect2DBeam(const CViewSegment2D & t_Beam);

        // Checks if segments intersects with the beam
        void checkSegment(const CViewSegment2D & t_Segment);

        [[nodiscard]] double Side() const;

        // Check if passed segment is part of the beam
        [[nodiscard]] std::optional<CViewSegment2D>
          getClosestCommonSegment(const CDirect2DBeam & t_Beam) const;

        [[nodiscard]] double cosAngle(const CViewSegment2D & t_Segment) const;

    private:
        // Checks if segment is aleardy part of beam hit
        [[nodiscard]] bool isSegmentIn(const CViewSegment2D & t_Segment) const;

        // Direct beam
        const CViewSegment2D m_Beam;

        // Segments that beam is intersecting with
        std::vector<CViewSegment2D> m_Segments;
    };

    ////////////////////////////////////////////////////////////////////////////////////////
    // CDirect2DRay
    ////////////////////////////////////////////////////////////////////////////////////////

    // Keeps information about direct ray. Ray is containing of two direct beams.
    class CDirect2DRay
    {
    public:
        CDirect2DRay(const CDirect2DBeam & t_Beam1, const CDirect2DBeam & t_Beam2);
        CDirect2DRay(const CViewSegment2D & t_Ray1, const CViewSegment2D & t_Ray2);

        // Returns ray height. Projection of the ray to the normal plane.
        [[nodiscard]] double rayNormalHeight() const;

        // Checks if segments intersects with the ray
        void checkSegment(const CViewSegment2D & t_Segment);

        // Return segment hit by the ray
        [[nodiscard]] std::optional<CViewSegment2D> closestSegmentHit() const;

        [[nodiscard]] double cosAngle(const CViewSegment2D & t_Segment) const;

    private:
        CDirect2DBeam m_Beam1;
        CDirect2DBeam m_Beam2;
    };

    ////////////////////////////////////////////////////////////////////////////////////////
    // CDirect2DRayResult
    ////////////////////////////////////////////////////////////////////////////////////////

    long long int keyFromProfileAngle(double angle);

    // Keeps result of beam ViewFactors. It is expensive operation to recalculate them every time
    // so this will just save results for the next call
    class CDirect2DRaysResult
    {
    public:
        CDirect2DRaysResult() = default;
        CDirect2DRaysResult(double t_ProfileAngle,
                            double t_DirectToDirect,
                            std::vector<BeamViewFactor> t_BeamViewFactors);

        // Beam view factors for given profile angle
        [[nodiscard]] std::vector<BeamViewFactor> beamViewFactors() const;

        // Direct to direct transmitted beam component
        [[nodiscard]] double directToDirect() const;

        [[nodiscard]] double profileAngle() const;

    private:
        std::vector<BeamViewFactor> m_ViewFactors;
        double m_DirectToDirect;
        double m_ProfileAngle;
    };

    ////////////////////////////////////////////////////////////////////////////////////////
    // CDirect2DRays
    ////////////////////////////////////////////////////////////////////////////////////////

    // Keeps information about group of direct rays entering or exiting the enclosure
    class CDirect2DRays
    {
    public:
        explicit CDirect2DRays(FenestrationCommon::Side t_Side);

        void appendGeometry2D(const CGeometry2D & t_Geometry2D);

        // Beam view factors for given profile angle
        std::vector<BeamViewFactor> beamViewFactors(double t_ProfileAngle);

        // Direct to direct transmitted beam component
        double directToDirect(double t_ProfileAngle);

    private:
        struct RayBoundaries
        {
            std::shared_ptr<CViewSegment2D> m_LowerRay;
            std::shared_ptr<CViewSegment2D> m_UpperRay;

            [[nodiscard]] bool isInRay(CPoint2D const & t_Point) const;
        };

        CDirect2DRaysResult calculateAllProperties(double t_ProfileAngle);

        // Finds lower and upper ray of every enclosure in the system
        RayBoundaries findRayBoundaries(double t_ProfileAngle);

        // Finds all points that are on the path of the ray
        std::vector<CDirect2DRay> findInBetweenRays(double t_ProfileAngle,
                                                    RayBoundaries & boudnaries);

        // Calculate beam view factors
        CDirect2DRaysResult calculateBeamProperties(double t_ProfileAngle,
                                                    std::vector<CDirect2DRay> & rays);


        [[nodiscard]] std::shared_ptr<CViewSegment2D> createSubBeam(CPoint2D const & t_Point,
                                                                    double t_ProfileAngle) const;

        FenestrationCommon::Side m_Side;

        std::vector<CGeometry2D> m_Geometries2D;

        std::map<long long, CDirect2DRaysResult> m_RayResults;
    };

    ////////////////////////////////////////////////////////////////////////////////////////
    // CGeometry2DBeam
    ////////////////////////////////////////////////////////////////////////////////////////

    // Class to handle beam view factors
    class CGeometry2DBeam
    {
    public:
        CGeometry2DBeam();

        void appendGeometry2D(const CGeometry2D & t_Geometry2D);

        std::vector<BeamViewFactor> beamViewFactors(double t_ProfileAngle,
                                                    FenestrationCommon::Side t_Side);

        // Direct to direct transmitted beam component
        double directToDirect(double t_ProfileAngle, FenestrationCommon::Side t_Side);

    private:
        std::map<FenestrationCommon::Side, CDirect2DRays> m_Ray;
    };

}   // namespace Viewer
