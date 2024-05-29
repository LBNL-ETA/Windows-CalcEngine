#include <stdexcept>
#include <cmath>
#include <cassert>

#include <WCEViewer.hpp>

#include "VenetianSlat.hpp"

using namespace Viewer;
using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    namespace Helper
    {
        void createPolarSegments(Viewer::CGeometry2D & aGeometry,
                                 const VenetianGeometry & venetian,
                                 size_t t_NumOfSegments,
                                 SegmentsDirection t_Direction,
                                 double radius)
        {
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
            double startTheta = t_Direction == SegmentsDirection::Positive ? theta2 : theta1;

            auto startPoint{CPoint2D::createPointFromPolarCoordinates(startTheta, radius)};
            for(size_t i = 1; i <= t_NumOfSegments; ++i)
            {
                double nextTheta = t_Direction == SegmentsDirection::Positive
                                     ? startTheta - dTheta * i
                                     : startTheta + dTheta * i;
                const auto endPoint{CPoint2D::createPointFromPolarCoordinates(nextTheta, radius)};
                CViewSegment2D aSegment{startPoint, endPoint};
                aGeometry.appendSegment(aSegment);
                startPoint = endPoint;
            }
        }

        void createCartesianSegments(Viewer::CGeometry2D & aGeometry,
                                     const VenetianGeometry & venetian,
                                     size_t t_NumOfSegments,
                                     SegmentsDirection t_Direction)
        {
            double dWidth = venetian.SlatWidth / t_NumOfSegments;
            double startRadius =
              t_Direction == SegmentsDirection::Positive ? 0 : dWidth * t_NumOfSegments;

            auto startPoint{
              CPoint2D::createPointFromPolarCoordinates(venetian.SlatTiltAngle, startRadius)};
            for(size_t i = 1; i <= t_NumOfSegments; ++i)
            {
                double nextRadius = t_Direction == SegmentsDirection::Positive
                                      ? i * dWidth
                                      : venetian.SlatWidth - i * dWidth;
                const auto endPoint{
                  CPoint2D::createPointFromPolarCoordinates(venetian.SlatTiltAngle, nextRadius)};
                CViewSegment2D aSegment{startPoint, endPoint};
                aGeometry.appendSegment(aSegment);
                startPoint = endPoint;
            }
        }

        std::pair<double, double> calculateTranslation(const Viewer::CGeometry2D & aGeometry,
                                                       SegmentsDirection t_Direction)
        {
            const CPoint2D aPoint{t_Direction == SegmentsDirection::Positive
                                    ? aGeometry.firstPoint()
                                    : aGeometry.lastPoint()};
            return {-aPoint.x(), -aPoint.y()};
        }
    }   // namespace Helper

    Viewer::CGeometry2D buildViewerSlat(const VenetianGeometry & t_Geometry,
                                        size_t t_NumOfSegments,
                                        SegmentsDirection t_Direction)
    {
        Viewer::CGeometry2D aGeometry;
        auto venetian{adjustSlatTiltAngle(t_Geometry)};
        double radius = std::abs(venetian.CurvatureRadius);

        if(radius > (venetian.SlatWidth / 2))
        {
            Helper::createPolarSegments(aGeometry, venetian, t_NumOfSegments, t_Direction, radius);
        }
        else if(radius == 0)
        {
            Helper::createCartesianSegments(aGeometry, venetian, t_NumOfSegments, t_Direction);
        }
        else
        {
            throw std::runtime_error("Cannot create slat.");
        }

        auto [translateX, translateY] = Helper::calculateTranslation(aGeometry, t_Direction);
        aGeometry = aGeometry.Translate(translateX, translateY + venetian.SlatSpacing);

        return aGeometry;
    }

}   // namespace SingleLayerOptics
