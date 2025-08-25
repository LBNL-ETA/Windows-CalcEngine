#include <cmath>
#include <vector>

#include "ColorProperties.hpp"
#include "CalculationProperties.hpp"
#include "IScatteringLayer.hpp"
#include <WCECommon.hpp>

namespace SingleLayerOptics
{
    Trichromatic::Trichromatic(double X, double Y, double Z) : X(X), Y(Y), Z(Z)
    {}

    aRGB::aRGB(int R, int G, int B) : R(R), G(G), B(B)
    {}

    CIE_LAB::CIE_LAB(double L, double A, double B) : L(L), a(A), b(B)
    {}

    ColorProperties::ColorProperties(std::unique_ptr<IScatteringLayer> && layerX,
                                     const FenestrationCommon::CSeries & t_Source,
                                     const FenestrationCommon::CSeries & t_DetectorX,
                                     const FenestrationCommon::CSeries & t_DetectorY,
                                     const FenestrationCommon::CSeries & t_DetectorZ,
                                     const std::vector<double> & t_wavelengths,
                                     FenestrationCommon::IntegrationType integrator,
                                     double normalizationCoefficient) :
        m_LayerX(std::move(layerX)),
        m_Source(t_Source),
        m_DetectorX(t_DetectorX),
        m_DetectorY(t_DetectorY),
        m_DetectorZ(t_DetectorZ),
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

        return CIE_LAB(116 * Q[1] - 16, 500 * (Q[0] - Q[1]), 200 * (Q[1] - Q[2]));
    }

}   // namespace SingleLayerOptics