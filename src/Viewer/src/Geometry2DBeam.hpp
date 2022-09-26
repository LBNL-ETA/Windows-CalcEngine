#ifndef GEOMETRY2DBEAM_H
#define GEOMETRY2DBEAM_H

#include <memory>
#include <vector>
#include <map>
#include <optional>

namespace FenestrationCommon
{
    enum class Side;
}

namespace Viewer
{
    class CViewSegment2D;
    class CGeometry2D;
    class CSegment2D;
    class CPoint2D;

    ////////////////////////////////////////////////////////////////////////////////////////
    // BeamViewFactor
    ////////////////////////////////////////////////////////////////////////////////////////
    struct BeamViewFactor
    {
        BeamViewFactor(size_t const t_Geometry2DIndex,
                       size_t const t_SegmentIndex,
                       double const t_Value,
                       double const t_PercentHit);
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
        explicit CDirect2DBeam(std::shared_ptr<const CViewSegment2D> const & t_Beam);

        // Checks if segments intersects with the beam
        void checkSegment(const CViewSegment2D & t_Segment);

        double Side() const;

        // Check if passed segment is part of the beam
        std::optional<CViewSegment2D> getClosestCommonSegment(const CDirect2DBeam & t_Beam) const;

        [[nodiscard]] double cosAngle(const CViewSegment2D & t_Segment) const;

    private:
        // Checks if segment is aleardy part of beam hit
        [[nodiscard]] bool isSegmentIn(const CViewSegment2D & t_Segment) const;

        // Direct beam
        std::shared_ptr<const CViewSegment2D> m_Beam;

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
        CDirect2DRay(std::shared_ptr<CDirect2DBeam> const & t_Beam1,
                     std::shared_ptr<CDirect2DBeam> const & t_Beam2);
        CDirect2DRay(std::shared_ptr<CViewSegment2D> const & t_Ray1,
                     std::shared_ptr<CViewSegment2D> const & t_Ray2);

        // Returns ray height. Projection of the ray to the normal plane.
        double rayNormalHeight() const;

        // Checks if segments intersects with the ray
        void checkSegment(const CViewSegment2D & t_Segment) const;

        // Return segment hit by the ray
        std::optional<CViewSegment2D> closestSegmentHit() const;

        double cosAngle(const CViewSegment2D & t_Segment) const;

    private:
        std::shared_ptr<CDirect2DBeam> m_Beam1;
        std::shared_ptr<CDirect2DBeam> m_Beam2;
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
        CDirect2DRaysResult(double const t_ProfileAngle,
                            double const t_DirectToDirect,
                            std::vector<BeamViewFactor> t_BeamViewFactors);

        // Beam view factors for given profile angle
        std::vector<BeamViewFactor> beamViewFactors() const;

        // Direct to direct transmitted beam component
        double directToDirect() const;

        double profileAngle() const;

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
        explicit CDirect2DRays(FenestrationCommon::Side const t_Side);

        void appendGeometry2D(std::shared_ptr<const CGeometry2D> const & t_Geometry2D);

        // Beam view factors for given profile angle
        std::vector<BeamViewFactor> beamViewFactors(double t_ProfileAngle);

        // Direct to direct transmitted beam component
        double directToDirect(double const t_ProfileAngle);

    private:
        struct RayBoundaries
        {
            std::shared_ptr<CViewSegment2D> m_LowerRay;
            std::shared_ptr<CViewSegment2D> m_UpperRay;

            bool isInRay(CPoint2D const & t_Point) const;
        };

        CDirect2DRaysResult calculateAllProperties(double const t_ProfileAngle);

        // Finds lower and upper ray of every enclosure in the system
        RayBoundaries findRayBoundaries(double const t_ProfileAngle);

        // Finds all points that are on the path of the ray
        std::vector<std::shared_ptr<CDirect2DRay>> findInBetweenRays(double const t_ProfileAngle,
                                                                     RayBoundaries & boudnaries);

        // Calculate beam view factors
        CDirect2DRaysResult
          calculateBeamProperties(double const t_ProfileAngle,
                                  std::vector<std::shared_ptr<CDirect2DRay>> & rays);


        std::shared_ptr<CViewSegment2D> createSubBeam(CPoint2D const & t_Point,
                                                      double const t_ProfileAngle) const;

        FenestrationCommon::Side m_Side;

        std::vector<std::shared_ptr<const CGeometry2D>> m_Geometries2D;

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

        void appendGeometry2D(std::shared_ptr<const CGeometry2D> const & t_Geometry2D);

        std::vector<BeamViewFactor> beamViewFactors(double const t_ProfileAngle,
                                                    FenestrationCommon::Side const t_Side);

        // Direct to direct transmitted beam component
        double directToDirect(double const t_ProfileAngle, FenestrationCommon::Side const t_Side);

    private:
        std::map<FenestrationCommon::Side, CDirect2DRays> m_Ray;
    };

}   // namespace Viewer

#endif
