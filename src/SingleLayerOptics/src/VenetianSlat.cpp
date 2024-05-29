#include <stdexcept>
#include <cmath>
#include <cassert>
#include <functional>

#include <WCEViewer.hpp>

#include "VenetianSlat.hpp"

using namespace Viewer;
using namespace FenestrationCommon;

namespace SingleLayerOptics
{
    namespace Helper
    {
        double calculateTheta(const VenetianGeometry & venetian, double radius)
        {
            return 2 * std::asin(venetian.SlatWidth / (2 * radius));
        }

        std::pair<double, double> calculateThetaBounds(const VenetianGeometry & venetian,
                                                       double theta)
        {
            using ConstantsData::WCE_PI;
            return {degrees((venetian.CurvatureRadius > 0 ? WCE_PI / 2 : -WCE_PI / 2)
                            + radians(venetian.SlatTiltAngle) - theta / 2),
                    degrees((venetian.CurvatureRadius > 0 ? WCE_PI / 2 : -WCE_PI / 2)
                            + radians(venetian.SlatTiltAngle) + theta / 2)};
        }

        Viewer::CGeometry2D createPolarSegments(const VenetianGeometry & venetian,
                                                size_t t_NumOfSegments,
                                                SegmentsDirection t_Direction)
        {
            auto [theta1, theta2]{calculateThetaBounds(
              venetian, calculateTheta(venetian, std::abs(venetian.CurvatureRadius)))};

            double dTheta = (theta2 - theta1) / t_NumOfSegments;
            double startTheta = t_Direction == SegmentsDirection::Positive ? theta2 : theta1;

            auto startPoint{CPoint2D::createPointFromPolarCoordinates(
              startTheta, std::abs(venetian.CurvatureRadius))};

            Viewer::CGeometry2D aGeometry;
            for(size_t i = 1u; i <= t_NumOfSegments; ++i)
            {
                double nextTheta = t_Direction == SegmentsDirection::Positive
                                     ? startTheta - dTheta * i
                                     : startTheta + dTheta * i;
                const auto endPoint{CPoint2D::createPointFromPolarCoordinates(
                  nextTheta, std::abs(venetian.CurvatureRadius))};
                aGeometry.appendSegment({startPoint, endPoint});
                startPoint = endPoint;
            }

            return aGeometry;
        }

        double getNextRadius(size_t segmentIndex,
                             double dWidth,
                             double slatWidth,
                             SegmentsDirection direction)
        {
            return direction == SegmentsDirection::Positive ? segmentIndex * dWidth
                                                            : slatWidth - segmentIndex * dWidth;
        }

        CPoint2D initializeStartPoint(const VenetianGeometry & venetian,
                                      SegmentsDirection direction,
                                      double dWidth,
                                      size_t numSegments)
        {
            return CPoint2D::createPointFromPolarCoordinates(
              venetian.SlatTiltAngle,
              direction == SegmentsDirection::Positive ? 0 : dWidth * numSegments);
        }

        CViewSegment2D createSegment(const VenetianGeometry &venetian,
                                     size_t segmentIndex,
                                     const CPoint2D &startPoint,
                                     double dWidth,
                                     SegmentsDirection t_Direction)
        {
            const auto endPoint = CPoint2D::createPointFromPolarCoordinates(
                venetian.SlatTiltAngle,
                getNextRadius(segmentIndex, dWidth, venetian.SlatWidth, t_Direction));
            return {startPoint, endPoint};
        }

         Viewer::CGeometry2D createCartesianSegments(const VenetianGeometry &venetian,
                                                    size_t t_NumOfSegments,
                                                    SegmentsDirection t_Direction)
        {
            double dWidth = venetian.SlatWidth / static_cast<double>(t_NumOfSegments);
            CPoint2D startPoint = initializeStartPoint(venetian, t_Direction, dWidth, t_NumOfSegments);

            Viewer::CGeometry2D aGeometry;
            for(size_t i = 1; i <= t_NumOfSegments; ++i)
            {
                auto segment = createSegment(venetian, i, startPoint, dWidth, t_Direction);
                aGeometry.appendSegment(createSegment(venetian, i, startPoint, dWidth, t_Direction));
                startPoint = segment.endPoint();
            }

            return aGeometry;
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

    Viewer::CGeometry2D buildViewerSlat(const VenetianGeometry & venetian,
                                        size_t t_NumOfSegments,
                                        SegmentsDirection t_Direction)
    {
        // clang-format off
        std::vector<std::pair<
                std::function<bool(const VenetianGeometry &)>,
                std::function<Viewer::CGeometry2D(const VenetianGeometry &, size_t, SegmentsDirection)>>
        > conditionFunctionVector = {
            {
                [](const VenetianGeometry &ven)
                {
                    return std::abs(ven.CurvatureRadius) > (ven.SlatWidth / 2);
                },
                Helper::createPolarSegments
            },
            {
                [](const VenetianGeometry &ven)
                {
                    return std::abs(ven.CurvatureRadius) == 0;
                },
                Helper::createCartesianSegments
            }
        };
        // clang-format on

        Viewer::CGeometry2D aGeometry;
        for(const auto & [condition, func] : conditionFunctionVector)
        {
            if(condition(venetian))
            {
                aGeometry = func(venetian, t_NumOfSegments, t_Direction);
                break;
            }
        }

        if(aGeometry.segments().empty())
        {
            throw std::runtime_error("Cannot create slat.");
        }

        auto [translateX, translateY]{Helper::calculateTranslation(aGeometry, t_Direction)};
        aGeometry = aGeometry.Translate(translateX, translateY + venetian.SlatSpacing);

        return aGeometry;
    }

}   // namespace SingleLayerOptics
