#ifndef WINDOWS_CALCENGINE_COLORPROPERTIES_HPP
#define WINDOWS_CALCENGINE_COLORPROPERTIES_HPP

#include <memory>
#include <vector>

#include "WCECommon.hpp"
#include "ScatteringLayer.hpp"

namespace SingleLayerOptics
{
    struct Trichromatic
    {
        Trichromatic(double X, double Y, double Z);

        double X;
        double Y;
        double Z;
    };

    struct aRGB
    {
        aRGB(int R, int G, int B);

        int R;
        int G;
        int B;
    };

    struct CIE_LAB
    {
        CIE_LAB(double L, double A, double B);

        double L;
        double a;
        double b;
    };

    class ColorProperties
    {
    public:
        /// For BSDF layers make additional constructor
        ColorProperties(std::unique_ptr<IScatteringLayer> && layerX,
                        const FenestrationCommon::CSeries & t_Source,
                        const FenestrationCommon::CSeries & t_DetectorX,
                        const FenestrationCommon::CSeries & t_DetectorY,
                        const FenestrationCommon::CSeries & t_DetectorZ,
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

    private:
        std::unique_ptr<IScatteringLayer> m_LayerX;
        FenestrationCommon::CSeries m_Source;
        FenestrationCommon::CSeries m_DetectorX;
        FenestrationCommon::CSeries m_DetectorY;
        FenestrationCommon::CSeries m_DetectorZ;
        std::vector<double> m_wavelengths;
        FenestrationCommon::IntegrationType m_Integrator;
        double m_NormalizationCoefficient;
        double m_SDx;
        double m_SDy;
        double m_SDz;
    };

}   // namespace SingleLayerOptics


#endif   // WINDOWS_CALCENGINE_COLORPROPERTIES_HPP
