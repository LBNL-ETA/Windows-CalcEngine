#pragma once

#include <memory>
#include <vector>

#include "WCECommon.hpp"
#include "ScatteringLayer.hpp"

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

    class ColorProperties
    {
    public:
        /// Constructor for color property calculations
        /// @param layerX The scattering layer to calculate properties for
        /// @param t_Source Source spectrum (e.g., D65 illuminant)
        /// @param t_DetectorX CIE color matching function X
        /// @param t_DetectorY CIE color matching function Y
        /// @param t_DetectorZ CIE color matching function Z
        /// @param t_LocusX Spectrum locus x chromaticity coordinates (wavelength vs x)
        /// @param t_LocusY Spectrum locus y chromaticity coordinates (wavelength vs y)
        /// @param t_wavelengths Optional wavelength set (uses layer wavelengths if empty)
        /// @param integrator Integration method
        /// @param normalizationCoefficient Normalization factor
        ColorProperties(std::unique_ptr<IScatteringLayer> && layerX,
                        const FenestrationCommon::CSeries & t_Source,
                        const FenestrationCommon::CSeries & t_DetectorX,
                        const FenestrationCommon::CSeries & t_DetectorY,
                        const FenestrationCommon::CSeries & t_DetectorZ,
                        const FenestrationCommon::CSeries & t_LocusX,
                        const FenestrationCommon::CSeries & t_LocusY,
                        const std::vector<double> & t_wavelengths = {},
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
