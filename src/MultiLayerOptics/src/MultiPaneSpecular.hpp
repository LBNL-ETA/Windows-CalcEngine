#pragma once

#include <memory>
#include <vector>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"
#include "EquivalentLayerSingleComponentMW.hpp"
#include "AbsorptancesMultiPane.hpp"

namespace SingleLayerOptics
{
    class CSpecularCell;

    class SpecularLayer;

}   // namespace SingleLayerOptics

namespace MultiLayerOptics
{
    class CEquivalentLayerSingleComponentMWAngle
    {
    public:
        CEquivalentLayerSingleComponentMWAngle(CEquivalentLayerSingleComponentMW t_Layer,
                                               CAbsorptancesMultiPane t_Abs,
                                               double t_Angle);

        double angle() const;

        const CEquivalentLayerSingleComponentMW & layer() const;

        FenestrationCommon::CSeries getProperties(FenestrationCommon::Side t_Side,
                                                  FenestrationCommon::Property t_Property);

        FenestrationCommon::CSeries Abs(size_t Index, FenestrationCommon::Side side);

        FenestrationCommon::CSeries iplus(size_t Index, FenestrationCommon::Side side);
        FenestrationCommon::CSeries iminus(size_t Index, FenestrationCommon::Side side);

    private:
        CEquivalentLayerSingleComponentMW m_Layer;
        CAbsorptancesMultiPane m_Abs;
        double m_Angle;
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    // CMultiPaneSpecular
    ///////////////////////////////////////////////////////////////////////////////////////

    // Handles equivalent properties of MultiLayerOptics glass consists only of specular layers
    class CMultiPaneSpecular : public SingleLayerOptics::IScatteringLayer
    {
    protected:
        explicit CMultiPaneSpecular(
          const std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> & t_Layer,
          const std::optional<std::vector<double>> & matrixWavelengths);

    public:
        static std::unique_ptr<CMultiPaneSpecular>
          create(const std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> & layers,
                 const std::optional<std::vector<double>> & matrixWavelengths = std::nullopt);


        double getPropertySimple(double minLambda,
                                 double maxLambda,
                                 FenestrationCommon::PropertySimple t_Property,
                                 FenestrationCommon::Side t_Side,
                                 FenestrationCommon::Scattering t_Scattering,
                                 double t_Theta = 0,
                                 double t_Phi = 0) override;

        [[nodiscard]] double getMinLambda() const override;

        [[nodiscard]] double getMaxLambda() const override;

        [[nodiscard]] std::vector<double> getWavelengths() const override;

        double getProperty(FenestrationCommon::Side t_Side,
                           FenestrationCommon::Property t_Property,
                           double t_Angle,
                           double minLambda,
                           double maxLambda,
                           FenestrationCommon::IntegrationType t_IntegrationType =
                             FenestrationCommon::IntegrationType::Trapezoidal,
                           double normalizationCoefficient = 1);

        double getHemisphericalProperty(FenestrationCommon::Side t_Side,
                                        FenestrationCommon::Property t_Property,
                                        const std::vector<double> & t_IntegrationAngles,
                                        double minLambda,
                                        double maxLambda,
                                        FenestrationCommon::IntegrationType t_IntegrationType =
                                          FenestrationCommon::IntegrationType::Trapezoidal,
                                        double normalizationCoefficient = 1);

        [[nodiscard]] size_t size() const;

        double getAbsorptanceLayer(double minLambda,
                                   double maxLambda,
                                   size_t index,
                                   FenestrationCommon::Side side,
                                   FenestrationCommon::ScatteringSimple scattering,
                                   double theta = 0,
                                   double phi = 0);

        double getAbsorptanceLayerHeat(double minLambda,
                                       double maxLambda,
                                       size_t index,
                                       FenestrationCommon::Side side,
                                       FenestrationCommon::ScatteringSimple scattering,
                                       double theta = 0,
                                       double phi = 0);

        double getAbsorptanceLayerElectricity(double minLambda,
                                              double maxLambda,
                                              size_t index,
                                              FenestrationCommon::Side side,
                                              FenestrationCommon::ScatteringSimple scattering,
                                              double theta = 0,
                                              double phi = 0);

        std::vector<double> getAbsorptanceLayers(double minLambda,
                                                 double maxLambda,
                                                 FenestrationCommon::Side side,
                                                 FenestrationCommon::ScatteringSimple scattering,
                                                 double theta = 0,
                                                 double phi = 0) override;

        std::vector<double>
          getAbsorptanceLayersHeat(const double minLambda,
                                   const double maxLambda,
                                   FenestrationCommon::Side side,
                                   FenestrationCommon::ScatteringSimple scattering,
                                   const double theta,
                                   const double phi) override;

        std::vector<double>
          getAbsorptanceLayersElectricity(const double minLambda,
                                          const double maxLambda,
                                          FenestrationCommon::Side side,
                                          FenestrationCommon::ScatteringSimple scattering,
                                          const double theta,
                                          const double phi) override;

        // Absorptances of each layer based on angle of incidence
        double Abs(size_t Index,
                   double t_Angle,
                   double minLambda,
                   double maxLambda,
                   FenestrationCommon::Side side,
                   FenestrationCommon::IntegrationType t_IntegrationType =
                     FenestrationCommon::IntegrationType::Trapezoidal,
                   double normalizationCoefficient = 1);

        double AbsHeat(size_t Index,
                       double t_Angle,
                       double minLambda,
                       double maxLambda,
                       FenestrationCommon::Side side,
                       FenestrationCommon::IntegrationType t_IntegrationType =
                         FenestrationCommon::IntegrationType::Trapezoidal,
                       double normalizationCoefficient = 1);

        double AbsElectricity(size_t Index,
                              double t_Angle,
                              double minLambda,
                              double maxLambda,
                              FenestrationCommon::Side side,
                              FenestrationCommon::IntegrationType t_IntegrationType =
                                FenestrationCommon::IntegrationType::Trapezoidal,
                              double normalizationCoefficient = 1);

        std::vector<double> Absorptances(double t_Angle,
                                         double minLambda,
                                         double maxLambda,
                                         FenestrationCommon::Side side,
                                         FenestrationCommon::IntegrationType t_IntegrationType =
                                           FenestrationCommon::IntegrationType::Trapezoidal,
                                         double normalizationCoefficient = 1);

        // Hemispherical absorptances of each layer. Integration is performed over t_Angles.
        double AbsHemispherical(size_t Index,
                                const std::vector<double> & t_IntegrationAngles,
                                double minLambda,
                                double maxLambda,
                                FenestrationCommon::Side side,
                                FenestrationCommon::IntegrationType t_IntegrationType =
                                  FenestrationCommon::IntegrationType::Trapezoidal,
                                double normalizationCoefficient = 1);

        double AbsHemisphericalHeat(size_t Index,
                                    const std::vector<double> & t_IntegrationAngles,
                                    double minLambda,
                                    double maxLambda,
                                    FenestrationCommon::Side side,
                                    FenestrationCommon::IntegrationType t_IntegrationType =
                                      FenestrationCommon::IntegrationType::Trapezoidal,
                                    double normalizationCoefficient = 1);

        double AbsHemisphericalElectricity(size_t Index,
                                           const std::vector<double> & t_IntegrationAngles,
                                           double minLambda,
                                           double maxLambda,
                                           FenestrationCommon::Side side,
                                           FenestrationCommon::IntegrationType t_IntegrationType =
                                             FenestrationCommon::IntegrationType::Trapezoidal,
                                           double normalizationCoefficient = 1);

        void setCalculationProperties(
          const SingleLayerOptics::CalculationProperties & calcProperties) override;

    protected:
        struct SeriesResults
        {
            FenestrationCommon::CSeries T;
            FenestrationCommon::CSeries Rf;
            FenestrationCommon::CSeries Rb;
        };

        // Get correct angular object out of array and if object does not exists, then it just
        // creates new one and stores it into array
        CEquivalentLayerSingleComponentMWAngle getAngular(double t_Angle);

        // creates equivalent layer properties for certain angle
        CEquivalentLayerSingleComponentMWAngle createNewAngular(double t_Angle);

        // Contains all specular layers (cells) that are added to the model. This way program will
        // be able to recalculate equivalent properties for any angle
        std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> m_Layers;

        std::vector<double> m_MatrixWavelengths;
        std::optional<std::vector<double>> m_SpectralIntegrationWavelengths;
        FenestrationCommon::CSeries m_ScaledSolarRadiation;

        // Results for angle-properties std::pair. If same angle is required twice, then model will
        // not calculate it twice. First it will search for results here and if results are not
        // available, then it will perform calculation for given angle
        std::vector<CEquivalentLayerSingleComponentMWAngle> m_EquivalentAngle;

        SeriesResults getSeriesResults(const SingleLayerOptics::CBeamDirection & aDirection,
                                       size_t layerIndex);

    private:
        static std::vector<double> unionOfLayerWavelengths(
          const std::vector<std::shared_ptr<SingleLayerOptics::SpecularLayer>> & t_Layer);
    };

}   // namespace MultiLayerOptics
