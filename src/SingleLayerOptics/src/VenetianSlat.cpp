#include <stdexcept>
#include <cmath>
#include <cassert>

#include <WCEViewer.hpp>
#include <WCECommon.hpp>

#include "VenetianSlat.hpp"

using namespace Viewer;
using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    CVenetianSlat::CVenetianSlat(const VenetianGeometry & t_Geometry,
                                 size_t t_NumOfSegments,
                                 SegmentsDirection t_Direction) :
        m_VenetianGeometry(t_Geometry)
    {
        buildSlat(t_Geometry, t_NumOfSegments, t_Direction);
    }

    CGeometry2D CVenetianSlat::geometry() const
    {
        return m_Geometry;
    }

    double CVenetianSlat::slatWidth() const
    {
        return m_VenetianGeometry.SlatWidth;
    }

    double CVenetianSlat::slatSpacing() const
    {
        return m_VenetianGeometry.SlatSpacing;
    }

    double CVenetianSlat::slatTiltAngle() const
    {
        return m_VenetianGeometry.SlatTiltAngle;
    }

    double CVenetianSlat::curvatureRadius() const
    {
        return m_VenetianGeometry.CurvatureRadius;
    }

    void CVenetianSlat::buildSlat(const VenetianGeometry & t_Geometry,
                                  const size_t t_NumOfSegments,
                                  const SegmentsDirection t_Direction)
    {
        auto venetian{FenestrationCommon::adjustSlatTiltAngle(t_Geometry)};

        double radius = std::abs(venetian.CurvatureRadius);
        double translateX = 0;
        double translateY = 0;

        if(radius > (venetian.SlatWidth / 2))
        {   // set properties in polar coordinate system
            using ConstantsData::WCE_PI;

            double theta = 2 * std::asin(venetian.SlatWidth / (2 * radius));
            double theta1 = 0;
            double theta2 = 0;
            double alpha = radians(venetian.SlatTiltAngle);

            if(venetian.CurvatureRadius > 0)
            {
                theta1 = degrees(WCE_PI / 2 + alpha - theta / 2);
                theta2 = degrees(WCE_PI / 2 + alpha + theta / 2);
            }
            else
            {
                theta1 = degrees(-WCE_PI / 2 + alpha + theta / 2);
                theta2 = degrees(-WCE_PI / 2 + alpha - theta / 2);
            }

            double dTheta = (theta2 - theta1) / t_NumOfSegments;
            double startTheta = 0;

            if(t_Direction == SegmentsDirection::Positive)
            {
                startTheta = theta2;
            }
            else if(t_Direction == SegmentsDirection::Negative)
            {
                startTheta = theta1;
            }
            else
            {
                assert("Incorrect selection for slat segments directions.");
            }


            auto startPoint{CPoint2D::createPointFromPolarCoordinates(startTheta, radius)};
            for(size_t i = 1; i <= t_NumOfSegments; ++i)
            {
                double nextTheta = 0;
                if(t_Direction == SegmentsDirection::Positive)
                {
                    nextTheta = startTheta - dTheta * i;
                }
                else if(t_Direction == SegmentsDirection::Negative)
                {
                    nextTheta = startTheta + dTheta * i;
                }

                const auto endPoint{CPoint2D::createPointFromPolarCoordinates(nextTheta, radius)};
                CViewSegment2D aSegment{startPoint, endPoint};
                m_Geometry.appendSegment(aSegment);
                startPoint = endPoint;
            }
        }
        else if(radius == 0)
        {
            double dWidth = venetian.SlatWidth / t_NumOfSegments;
            double startRadius = 0;
            if(t_Direction == SegmentsDirection::Positive)
            {
                startRadius = 0;
            }
            else if(t_Direction == SegmentsDirection::Negative)
            {
                startRadius = dWidth * t_NumOfSegments;
            }
            else
            {
                assert("Incorrect selection for slat segments directions.");
            }

            auto startPoint{
              CPoint2D::createPointFromPolarCoordinates(venetian.SlatTiltAngle, startRadius)};
            for(size_t i = 1; i <= t_NumOfSegments; ++i)
            {
                double nextRadius = 0;
                if(t_Direction == SegmentsDirection::Positive)
                {
                    nextRadius = i * dWidth;
                }
                else if(t_Direction == SegmentsDirection::Negative)
                {
                    nextRadius = venetian.SlatWidth - i * dWidth;
                }
                const auto endPoint{
                  CPoint2D::createPointFromPolarCoordinates(venetian.SlatTiltAngle, nextRadius)};
                CViewSegment2D aSegment{startPoint, endPoint};
                m_Geometry.appendSegment(aSegment);
                startPoint = endPoint;
            }
        }
        else
        {
            throw std::runtime_error("Cannot create slat.");
        }

        const CPoint2D aPoint{t_Direction == SegmentsDirection::Positive ? m_Geometry.firstPoint()
                                                                         : m_Geometry.lastPoint()};

        translateX = -aPoint.x();
        translateY = -aPoint.y();

        m_Geometry = m_Geometry.Translate(translateX, translateY + venetian.SlatSpacing);
    }

}   // namespace SingleLayerOptics
