#include <cmath>
#include <optional>
#include <tuple>
#include <vector>

#include "ColorProperties.hpp"
#include "CalculationProperties.hpp"
#include "IScatteringLayer.hpp"
#include <WCECommon.hpp>

namespace SingleLayerOptics
{
    namespace
    {
        struct Point2D
        {
            double x = 0.0;
            double y = 0.0;

            [[nodiscard]] double distanceTo(const Point2D & other) const
            {
                const double deltaX = other.x - x;
                const double deltaY = other.y - y;
                return std::sqrt(deltaX * deltaX + deltaY * deltaY);
            }
        };

        struct LineSegment
        {
            Point2D start;
            Point2D end;
        };

        /// Find intersection of a ray with a line segment.
        /// Ray starts at rayOrigin and passes through rayThrough.
        /// Returns intersection point if it exists (within segment and in ray's forward direction).
        [[nodiscard]] std::optional<Point2D> raySegmentIntersection(const Point2D & rayOrigin,
                                                                    const Point2D & rayThrough,
                                                                    const LineSegment & segment)
        {
            const double denom = (rayOrigin.x - rayThrough.x) * (segment.start.y - segment.end.y)
                                 - (rayOrigin.y - rayThrough.y) * (segment.start.x - segment.end.x);
            if(std::abs(denom) < 1e-10)
            {
                return std::nullopt;   // Lines are parallel
            }

            const double tNum = (rayOrigin.x - segment.start.x) * (segment.start.y - segment.end.y)
                                - (rayOrigin.y - segment.start.y) * (segment.start.x - segment.end.x);
            const double uNum = -((rayOrigin.x - rayThrough.x) * (rayOrigin.y - segment.start.y)
                                  - (rayOrigin.y - rayThrough.y) * (rayOrigin.x - segment.start.x));

            const double paramT = tNum / denom;
            const double paramU = uNum / denom;

            // paramU in [0,1] means intersection is within segment; paramT > 0 means forward along ray
            if(paramU >= 0.0 && paramU <= 1.0 && paramT > 0.0)
            {
                return Point2D{rayOrigin.x + paramT * (rayThrough.x - rayOrigin.x),
                               rayOrigin.y + paramT * (rayThrough.y - rayOrigin.y)};
            }
            return std::nullopt;
        }

        /// Extract a locus point (chromaticity coordinates) at given index
        [[nodiscard]] Point2D getLocusPoint(const FenestrationCommon::CSeries & locusX,
                                            const FenestrationCommon::CSeries & locusY,
                                            size_t index)
        {
            return Point2D{locusX[index].value(), locusY[index].value()};
        }

        /// Find intersection of line from white point through sample with spectrum locus.
        /// Returns wavelength (in same units as locus data) and the intersection point.
        [[nodiscard]] std::tuple<double, Point2D>
          findSpectrumLocusIntersection(const Point2D & sample,
                                        const Point2D & white,
                                        const FenestrationCommon::CSeries & locusX,
                                        const FenestrationCommon::CSeries & locusY)
        {
            const size_t numPoints = std::min(locusX.size(), locusY.size());

            // Search for intersection with spectrum locus segments
            for(size_t idx = 0; idx < numPoints - 1; ++idx)
            {
                const Point2D locusStart = getLocusPoint(locusX, locusY, idx);
                const Point2D locusEnd = getLocusPoint(locusX, locusY, idx + 1);

                if(auto intersection = raySegmentIntersection(white, sample, {locusStart, locusEnd}))
                {
                    // Interpolate wavelength based on position along segment
                    const double segDist = locusStart.distanceTo(locusEnd);
                    const double intDist = locusStart.distanceTo(*intersection);
                    const double ratio = (segDist > 1e-10) ? intDist / segDist : 0.0;
                    const double wvl1 = locusX[idx].x();
                    const double wvl2 = locusX[idx + 1].x();
                    return {wvl1 + ratio * (wvl2 - wvl1), *intersection};
                }
            }

            // Check purple line (connection between red and blue ends of spectrum locus)
            if(numPoints >= 2)
            {
                const Point2D blueEnd = getLocusPoint(locusX, locusY, 0);
                const Point2D redEnd = getLocusPoint(locusX, locusY, numPoints - 1);

                if(auto intersection = raySegmentIntersection(white, sample, {blueEnd, redEnd}))
                {
                    // For purple colors, return complementary wavelength (negative by convention)
                    const double wvlBlue = locusX[0].x();
                    const double wvlRed = locusX[numPoints - 1].x();
                    const double wavelength = (intersection->distanceTo(blueEnd) < intersection->distanceTo(redEnd))
                                                ? -wvlBlue
                                                : -wvlRed;
                    return {wavelength, *intersection};
                }
            }

            // Fallback - should not reach here for valid colors
            return {0.0, sample};
        }
    }   // namespace

    ColorProperties::ColorProperties(std::unique_ptr<IScatteringLayer> && layerX,
                                     const FenestrationCommon::CSeries & t_Source,
                                     const FenestrationCommon::CSeries & t_DetectorX,
                                     const FenestrationCommon::CSeries & t_DetectorY,
                                     const FenestrationCommon::CSeries & t_DetectorZ,
                                     const FenestrationCommon::CSeries & t_LocusX,
                                     const FenestrationCommon::CSeries & t_LocusY,
                                     const std::vector<double> & t_wavelengths,
                                     FenestrationCommon::IntegrationType integrator,
                                     double normalizationCoefficient) :
        m_LayerX(std::move(layerX)),
        m_Source(t_Source),
        m_DetectorX(t_DetectorX),
        m_DetectorY(t_DetectorY),
        m_DetectorZ(t_DetectorZ),
        m_LocusX(t_LocusX),
        m_LocusY(t_LocusY),
        m_wavelengths(t_wavelengths),
        m_Integrator(integrator),
        m_NormalizationCoefficient(normalizationCoefficient)
    {
        if(t_wavelengths.empty())
        {
            m_wavelengths = m_LayerX->getWavelengths();
        }
        auto aSolar = t_Source;
        auto DX = t_DetectorX;
        auto DY = t_DetectorY;
        auto DZ = t_DetectorZ;
        aSolar = aSolar.interpolate(m_wavelengths);
        DX = DX.interpolate(m_wavelengths);
        DY = DY.interpolate(m_wavelengths);
        DZ = DZ.interpolate(m_wavelengths);

        m_SDx = (aSolar * DX).sum(m_LayerX->getMinLambda(), m_LayerX->getMaxLambda());
        m_SDy = (aSolar * DY).sum(m_LayerX->getMinLambda(), m_LayerX->getMaxLambda());
        m_SDz = (aSolar * DZ).sum(m_LayerX->getMinLambda(), m_LayerX->getMaxLambda());
    }

    Trichromatic
      ColorProperties::getTrichromatic(const FenestrationCommon::PropertySurface t_Property,
                                       const FenestrationCommon::Side t_Side,
                                       const FenestrationCommon::Scattering t_Scattering,
                                       double const t_Theta,
                                       double const t_Phi)
    {
        SingleLayerOptics::CalculationProperties inputX{
          m_Source, m_wavelengths, m_DetectorX, m_Integrator, m_NormalizationCoefficient};
        m_LayerX->setCalculationProperties(inputX);
        auto X = m_SDx / m_SDy * 100
                 * m_LayerX->getPropertySurface(m_LayerX->getMinLambda(),
                                                m_LayerX->getMaxLambda(),
                                                t_Property,
                                                t_Side,
                                                t_Scattering,
                                                t_Theta,
                                                t_Phi);

        SingleLayerOptics::CalculationProperties inputY{
          m_Source, m_wavelengths, m_DetectorY, m_Integrator, m_NormalizationCoefficient};
        m_LayerX->setCalculationProperties(inputY);
        auto Y = 100
                 * m_LayerX->getPropertySurface(m_LayerX->getMinLambda(),
                                                m_LayerX->getMaxLambda(),
                                                t_Property,
                                                t_Side,
                                                t_Scattering,
                                                t_Theta,
                                                t_Phi);

        SingleLayerOptics::CalculationProperties inputZ{
          m_Source, m_wavelengths, m_DetectorZ, m_Integrator, m_NormalizationCoefficient};
        m_LayerX->setCalculationProperties(inputZ);
        auto Z = m_SDz / m_SDy * 100
                 * m_LayerX->getPropertySurface(m_LayerX->getMinLambda(),
                                                m_LayerX->getMaxLambda(),
                                                t_Property,
                                                t_Side,
                                                t_Scattering,
                                                t_Theta,
                                                t_Phi);
        return Trichromatic(X, Y, Z);
    }

    aRGB ColorProperties::getRGB(const FenestrationCommon::PropertySurface t_Property,
                                 const FenestrationCommon::Side t_Side,
                                 const FenestrationCommon::Scattering t_Scattering,
                                 double const t_Theta,
                                 double const t_Phi)
    {
        auto tri = getTrichromatic(t_Property, t_Side, t_Scattering, t_Theta, t_Phi);
        auto X = 0.0125313 * (tri.X - 0.1901);
        auto Y = 0.0125313 * (tri.Y - 0.2);
        auto Z = 0.0125313 * (tri.Z - 0.2178);

        /// According to https://www.w3.org/Graphics/Color/srgb.pdf
        const FenestrationCommon::SquareMatrix T({{3.2406255, -1.537208, -0.4986286},
                                                  {-0.9689307, 1.8757561, 0.0415175},
                                                  {0.0557101, -0.2040211, 1.0569959}});

        const std::vector<double> xyz({X, Y, Z});
        auto mmult = T * xyz;

        const double testlimit = 0.0031308;
        for(auto & val : mmult)
        {
            val = (val <= testlimit) ? val * 12.92 : 1.055 * std::pow(val, 1 / 2.4) - 0.055;
            if(val > 1.0)
                val = 1.0;
            if(val < 0.0)
                val = 0.0;
            val = val * 255;
        }

        auto R = int(std::lround(mmult[0]));
        auto G = int(std::lround(mmult[1]));
        auto B = int(std::lround(mmult[2]));

        return aRGB(R, G, B);
    }

    CIE_LAB ColorProperties::getCIE_Lab(const FenestrationCommon::PropertySurface t_Property,
                                        const FenestrationCommon::Side t_Side,
                                        const FenestrationCommon::Scattering t_Scattering,
                                        double const t_Theta,
                                        double const t_Phi)
    {
        SingleLayerOptics::CalculationProperties inputX{
          m_Source, m_wavelengths, m_DetectorX, m_Integrator, m_NormalizationCoefficient};
        m_LayerX->setCalculationProperties(inputX);
        auto X = m_LayerX->getPropertySurface(m_LayerX->getMinLambda(),
                                              m_LayerX->getMaxLambda(),
                                              t_Property,
                                              t_Side,
                                              t_Scattering,
                                              t_Theta,
                                              t_Phi);

        SingleLayerOptics::CalculationProperties inputY{
          m_Source, m_wavelengths, m_DetectorY, m_Integrator, m_NormalizationCoefficient};
        m_LayerX->setCalculationProperties(inputY);
        auto Y = m_LayerX->getPropertySurface(m_LayerX->getMinLambda(),
                                              m_LayerX->getMaxLambda(),
                                              t_Property,
                                              t_Side,
                                              t_Scattering,
                                              t_Theta,
                                              t_Phi);

        SingleLayerOptics::CalculationProperties inputZ{
          m_Source, m_wavelengths, m_DetectorZ, m_Integrator, m_NormalizationCoefficient};
        m_LayerX->setCalculationProperties(inputZ);
        auto Z = m_LayerX->getPropertySurface(m_LayerX->getMinLambda(),
                                              m_LayerX->getMaxLambda(),
                                              t_Property,
                                              t_Side,
                                              t_Scattering,
                                              t_Theta,
                                              t_Phi);

        std::vector<double> Q{X, Y, Z};
        for(auto & val : Q)
        {
            val = (val > std::pow(6.0 / 29.0, 3)) ? std::pow(val, 1.0 / 3.0)
                                                  : (841.0 / 108.0) * val + 4.0 / 29.0;
        }

        return CIE_LAB{116 * Q[1] - 16, 500 * (Q[0] - Q[1]), 200 * (Q[1] - Q[2])};
    }

    DominantWavelengthPurity ColorProperties::getDominantWavelengthAndPurity(
      const FenestrationCommon::PropertySurface t_Property,
      const FenestrationCommon::Side t_Side,
      const FenestrationCommon::Scattering t_Scattering,
      double const t_Theta,
      double const t_Phi)
    {
        // Get tristimulus values
        const auto [triX, triY, triZ] =
          getTrichromatic(t_Property, t_Side, t_Scattering, t_Theta, t_Phi);

        // Convert to chromaticity coordinates
        const double sum = triX + triY + triZ;
        if(sum < 1e-10)
        {
            return DominantWavelengthPurity{0.0, 0.0};
        }

        const Point2D sample{triX / sum, triY / sum};

        // Calculate white point chromaticity from source tristimulus values
        const double whiteSum = m_SDx + m_SDy + m_SDz;
        const Point2D white{m_SDx / whiteSum, m_SDy / whiteSum};

        // Find intersection with spectrum locus
        const auto [wavelength, locusPoint] =
          findSpectrumLocusIntersection(sample, white, m_LocusX, m_LocusY);

        // Calculate excitation purity using the same formula as the reference VB implementation:
        // Purity = dist(sample, white) / (dist(sample, locus) + dist(sample, white)) * 100%
        const double distSampleToLocus = sample.distanceTo(locusPoint);
        const double distSampleToWhite = sample.distanceTo(white);

        double purity = 0.0;
        if(distSampleToLocus + distSampleToWhite > 1e-10)
        {
            purity = (distSampleToWhite / (distSampleToLocus + distSampleToWhite)) * 100.0;
        }

        // Wavelength is already in microns (from locus data), take absolute value for purple colors
        return DominantWavelengthPurity{std::abs(wavelength), purity};
    }

}   // namespace SingleLayerOptics