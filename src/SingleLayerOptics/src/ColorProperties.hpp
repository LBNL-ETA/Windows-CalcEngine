#pragma once

#include <memory>
#include <vector>

#include "WCECommon.hpp"
#include "ScatteringLayer.hpp"
#include "SpectrumLocus.hpp"

namespace SingleLayerOptics
{
    struct Trichromatic
    {
        Trichromatic() = default;
        Trichromatic(double xVal, double yVal, double zVal) : X(xVal), Y(yVal), Z(zVal)
        {}

        double X{0};
        double Y{0};
        double Z{0};
    };

    struct aRGB
    {
        aRGB() = default;
        aRGB(int rVal, int gVal, int bVal) : R(rVal), G(gVal), B(bVal)
        {}

        int R{0};
        int G{0};
        int B{0};
    };

    struct CIE_LAB
    {
        CIE_LAB() = default;
        CIE_LAB(double lVal, double aVal, double bVal) : L(lVal), a(aVal), b(bVal)
        {}

        double L{0};
        double a{0};
        double b{0};
    };

    /// Dominant wavelength (in micrometers) and excitation purity (percentage)
    struct DominantWavelengthPurity
    {
        DominantWavelengthPurity() = default;
        DominantWavelengthPurity(double wavelength, double pur) :
            dominantWavelength(wavelength), purity(pur)
        {}

        double dominantWavelength{0};   // [μm]
        double purity{0};               // [%]
    };

    /// CIE color matching functions (tristimulus weighting functions)
    struct ColorDetectors
    {
        FenestrationCommon::CSeries X;
        FenestrationCommon::CSeries Y;
        FenestrationCommon::CSeries Z;
    };

    /// Spectrum locus chromaticity coordinates for dominant wavelength calculation
    struct SpectrumLocus
    {
        FenestrationCommon::CSeries X;
        FenestrationCommon::CSeries Y;
    };

    class ColorProperties
    {
    public:
        /// Constructor for color property calculations
        /// @param layer The scattering layer to calculate properties for
        /// @param source Source spectrum (e.g., D65 illuminant)
        /// @param detectors CIE color matching functions (X, Y, Z)
        /// @param locus Spectrum locus chromaticity coordinates (defaults to CIE 1964)
        /// @param wavelengths Optional wavelength set (uses layer wavelengths if empty)
        /// @param integrator Integration method
        /// @param normalizationCoefficient Normalization factor
        ColorProperties(std::unique_ptr<IScatteringLayer> && layer,
                        const FenestrationCommon::CSeries & source,
                        const ColorDetectors & detectors,
                        const SpectrumLocus & locus = {CIE_1964_Locus_X(), CIE_1964_Locus_Y()},
                        const std::vector<double> & wavelengths = {},
                        FenestrationCommon::IntegrationType integrator =
                          FenestrationCommon::IntegrationType::Trapezoidal,
                        double normalizationCoefficient = 1.0);

        Trichromatic getTrichromatic(FenestrationCommon::PropertySurface t_Property,
                                     FenestrationCommon::Side t_Side,
                                     FenestrationCommon::Scattering t_Scattering,
                                     double t_Theta = 0,
                                     double t_Phi = 0);

        aRGB getRGB(FenestrationCommon::PropertySurface t_Property,
                    FenestrationCommon::Side t_Side,
                    FenestrationCommon::Scattering t_Scattering,
                    double t_Theta = 0,
                    double t_Phi = 0);

        CIE_LAB getCIE_Lab(FenestrationCommon::PropertySurface t_Property,
                           FenestrationCommon::Side t_Side,
                           FenestrationCommon::Scattering t_Scattering,
                           double t_Theta = 0,
                           double t_Phi = 0);

        DominantWavelengthPurity getDominantWavelengthAndPurity(
          FenestrationCommon::PropertySurface t_Property,
          FenestrationCommon::Side t_Side,
          FenestrationCommon::Scattering t_Scattering,
          double t_Theta = 0,
          double t_Phi = 0);

    private:
        std::unique_ptr<IScatteringLayer> m_LayerX;
        FenestrationCommon::CSeries m_Source;
        FenestrationCommon::CSeries m_DetectorX;
        FenestrationCommon::CSeries m_DetectorY;
        FenestrationCommon::CSeries m_DetectorZ;
        FenestrationCommon::CSeries m_LocusX;   // Spectrum locus x chromaticity
        FenestrationCommon::CSeries m_LocusY;   // Spectrum locus y chromaticity
        std::vector<double> m_wavelengths;
        FenestrationCommon::IntegrationType m_Integrator;
        double m_NormalizationCoefficient;
        double m_SDx;
        double m_SDy;
        double m_SDz;
    };

}   // namespace SingleLayerOptics
