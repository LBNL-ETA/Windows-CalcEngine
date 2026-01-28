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
        double X;
        double Y;
        double Z;
    };

    struct aRGB
    {
        int R;
        int G;
        int B;
    };

    struct CIE_LAB
    {
        double L;
        double a;
        double b;
    };

    /// Dominant wavelength (in micrometers) and excitation purity (percentage)
    struct DominantWavelengthPurity
    {
        double dominantWavelength;   // [μm]
        double purity;               // [%]
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
