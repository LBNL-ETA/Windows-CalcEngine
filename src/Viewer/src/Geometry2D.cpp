#include <cassert>

#include "Geometry2D.hpp"
#include "ViewSegment2D.hpp"
#include "Point2D.hpp"
#include "ViewerConstants.hpp"


using namespace FenestrationCommon;

namespace Viewer
{
    CGeometry2D::CGeometry2D() : m_ViewFactorsCalculated(false)
    {}

    void CGeometry2D::appendSegment(const CViewSegment2D & t_Segment)
    {
        m_Segments.push_back(t_Segment);
        m_ViewFactorsCalculated = false;
    }

    void CGeometry2D::appendGeometry2D(const CGeometry2D & t_Geometry2D)
    {
        for(auto aSegment : (t_Geometry2D.m_Segments))
        {
            m_Segments.push_back(aSegment);
        }
        m_ViewFactorsCalculated = false;
    }

    SquareMatrix CGeometry2D::viewFactors()
    {
        checkViewFactors();

        return m_ViewFactors;
    }

    CGeometry2D CGeometry2D::Translate(double const t_x, double const t_y) const
    {
        CGeometry2D aEnclosure;
        for(auto aSegment : m_Segments)
        {
            const auto newSegment{aSegment.translate(t_x, t_y)};
            CViewSegment2D newEnSegment{newSegment.startPoint(), newSegment.endPoint()};
            aEnclosure.appendSegment(newEnSegment);
        }

        return aEnclosure;
    }

    CPoint2D CGeometry2D::firstPoint() const
    {
        return m_Segments.front().startPoint();
    }

    CPoint2D CGeometry2D::lastPoint() const
    {
        return m_Segments.back().endPoint();
    }

    CPoint2D CGeometry2D::entryPoint() const
    {
        auto xStart = m_Segments.front().centerPoint().x();
        auto xEnd = m_Segments.back().centerPoint().x();
        const CPoint2D startPoint{xStart <= xEnd ? m_Segments.front().startPoint()
                                                 : m_Segments.back().startPoint()};
        const CPoint2D endPoint{xStart <= xEnd ? m_Segments.front().endPoint()
                                               : m_Segments.back().endPoint()};

        return startPoint.x() < endPoint.x() ? startPoint : endPoint;
    }

    CPoint2D CGeometry2D::exitPoint() const
    {
        auto xStart = m_Segments.front().centerPoint().x();
        auto xEnd = m_Segments.back().centerPoint().x();
        const CPoint2D startPoint{xStart >= xEnd ? m_Segments.front().startPoint()
                                                 : m_Segments.back().startPoint()};
        const CPoint2D endPoint{xStart >= xEnd ? m_Segments.front().endPoint()
                                               : m_Segments.back().endPoint()};

        return startPoint.x() > endPoint.x() ? startPoint : endPoint;
    }

    const std::vector<CViewSegment2D> & CGeometry2D::segments() const
    {
        return m_Segments;
    }

    bool CGeometry2D::pointInSegmentsView(CViewSegment2D const & t_Segment1,
                                          CViewSegment2D const & t_Segment2,
                                          CPoint2D const & t_Point)
    {
        // Forming polygon
        std::vector<CViewSegment2D> aPolygon;
        aPolygon.push_back(t_Segment1);
        auto point1 = t_Segment1.endPoint();
        auto point2 = t_Segment2.startPoint();
        auto aSide2 = CViewSegment2D(point1, point2);
        if(aSide2.length() > 0)
        {
            aPolygon.push_back(aSide2);
        }
        aPolygon.push_back(t_Segment2);
        auto point3 = t_Segment2.endPoint();
        auto point4 = t_Segment1.startPoint();
        auto aSide4 = CViewSegment2D(point3, point4);
        if(aSide4.length() > 0)
        {
            aPolygon.push_back(aSide4);
        }

        // now check if point is in the polygon. Note that if point is of any edge of the polygon,
        // it will not be considered to be part of blocking surface. Otherwise, program would search
        // for blocking surfaces and perform double integration over the both surfaces.
        auto inSide = true;
        for(auto aSegment : aPolygon)
        {
            inSide = inSide && (aSegment.position(t_Point) == PointPosition::Visible);
            if(!inSide)
            {
                break;
            }
        }

        return inSide;
    }

    bool CGeometry2D::thirdSurfaceShadowing(CViewSegment2D const & t_Segment1,
                                            CViewSegment2D const & t_Segment2) const
    {
        auto intersection = false;

        // Form cross segments
        std::vector<std::shared_ptr<CViewSegment2D>> intSegments;
        auto r11 = std::make_shared<CViewSegment2D>(t_Segment1.startPoint(), t_Segment2.endPoint());
        if(r11->length() > 0)
        {
            intSegments.push_back(r11);
        }
        auto r22 = std::make_shared<CViewSegment2D>(t_Segment1.endPoint(), t_Segment2.startPoint());
        if(r22->length() > 0)
        {
            intSegments.push_back(r22);
        }

        for(auto aSegment : m_Segments)
        {
            for(auto iSegment : intSegments)
            {
                if(aSegment != t_Segment1 && aSegment != t_Segment2)
                {
                    intersection = intersection || iSegment->intersectionWithSegment(aSegment);
                    intersection =
                      intersection
                      || pointInSegmentsView(t_Segment1, t_Segment2, aSegment.startPoint());
                    intersection =
                      intersection
                      || pointInSegmentsView(t_Segment1, t_Segment2, aSegment.endPoint());
                    if(intersection)
                    {
                        return intersection;
                    }
                }
            }
        }

        return intersection;
    }

    bool CGeometry2D::thirdSurfaceShadowingSimple(const CViewSegment2D & t_Segment1,
                                                  const CViewSegment2D & t_Segment2) const
    {
        auto intersection = false;

        CViewSegment2D centerLine{t_Segment1.centerPoint(), t_Segment2.centerPoint()};

        for(auto aSegment : m_Segments)
        {
            if(aSegment != t_Segment1 && aSegment != t_Segment2)
            {
                intersection = intersection || centerLine.intersectionWithSegment(aSegment);
                if(intersection)
                {
                    break;
                }
            }
        }

        return intersection;
    }

    double CGeometry2D::viewFactorCoeff(const CViewSegment2D & t_Segment1,
                                        const CViewSegment2D & t_Segment2) const
    {
        auto subViewCoeff = 0.0;

        auto subSeg1 = t_Segment1.subSegments(ViewerConstants::NUM_OF_SEGMENTS);
        auto subSeg2 = t_Segment2.subSegments(ViewerConstants::NUM_OF_SEGMENTS);

        for(const auto & sub1 : subSeg1)
        {
            for(const auto & sub2 : subSeg2)
            {
                auto selfShadowing = sub1.selfShadowing(sub2);
                auto tSurfBlock = thirdSurfaceShadowingSimple(sub1, sub2);
                if(!tSurfBlock && selfShadowing == Shadowing::No)
                {
                    auto cVF = sub1.viewFactorCoefficient(sub2);
                    subViewCoeff += cVF;
                }
            }
        }

        if(subViewCoeff < ViewerConstants::MIN_VIEW_COEFF)
        {
            subViewCoeff = 0;
        }

        return subViewCoeff;
    }

    // Not used for now. Commented to avoid warnings
    // double CGeometry2D::intersectionWithYAxis( double const tanPhi, CPoint2D const& t_Point ) {
    // 	auto y = 0.0;
    // 	if ( tanPhi != 0 ) {
    // 		auto x1 = t_Point.y() / tanPhi;
    // 		auto x = x1 + t_Point.x();
    // 		y = tanPhi * x;
    // 	}
    // 	else {
    // 		y = t_Point.y();
    // 	}
    //
    // 	return y;
    // }

    void CGeometry2D::checkViewFactors()
    {
        if(!m_ViewFactorsCalculated)
        {
            auto size = m_Segments.size();

            // View factor matrix. It is already initialized to zeros
            m_ViewFactors = SquareMatrix(size);
            for(auto i = 0u; i < size; ++i)
            {
                for(auto j = i; j < size; ++j)
                {
                    if(i != j)
                    {
                        auto selfShadowing = m_Segments[i].selfShadowing(m_Segments[j]);
                        if(selfShadowing != Shadowing::Total)
                        {
                            auto shadowedByThirdSurface =
                              thirdSurfaceShadowing(m_Segments[i], m_Segments[j]);
                            auto vfCoeff = 0.0;

                            if(!shadowedByThirdSurface && (selfShadowing == Shadowing::No))
                            {
                                vfCoeff = m_Segments[i].viewFactorCoefficient(m_Segments[j]);
                            }
                            else if(shadowedByThirdSurface || selfShadowing == Shadowing::Partial)
                            {
                                vfCoeff = viewFactorCoeff(m_Segments[i], m_Segments[j]);
                            }

                            m_ViewFactors(i, j) = vfCoeff / (2 * m_Segments[i].length());
                            m_ViewFactors(j, i) = vfCoeff / (2 * m_Segments[j].length());
                        }
                    }
                }
            }

            m_ViewFactorsCalculated = true;
        }
    }

}   // namespace Viewer
