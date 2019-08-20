#ifndef MultiPaneSpecular_H
#define MultiPaneSpecular_H

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
        FenestrationCommon::CSeries
        getProperties(const FenestrationCommon::Side t_Side, const FenestrationCommon::Property t_Property);
        FenestrationCommon::CSeries Abs(const size_t Index);

    private:
        CEquivalentLayerSingleComponentMW m_Layer;
        CAbsorptancesMultiPane m_Abs;
        double m_Angle;
    };

    // Handles equivalent properties of MultiLayerOptics glass consists only of specular layers
    class CMultiPaneSpecular : public SingleLayerOptics::IScatteringLayer
    {
    private:
        CMultiPaneSpecular(
          std::vector<SingleLayerOptics::SpecularLayer> layers,
          const std::shared_ptr<FenestrationCommon::CSeries> & t_SolarRadiation,
          const std::shared_ptr<FenestrationCommon::CSeries> & t_DetectorData = nullptr);

        CMultiPaneSpecular(std::vector<double> const & t_CommonWavelength,
                           const std::shared_ptr<FenestrationCommon::CSeries> & t_SolarRadiation,
                           SingleLayerOptics::SpecularLayer & t_Layer);

        void addLayer(SingleLayerOptics::SpecularLayer & t_Layer);

    public:
        static std::unique_ptr<CMultiPaneSpecular>
          create(std::vector<SingleLayerOptics::SpecularLayer> layers,
                 const std::shared_ptr<FenestrationCommon::CSeries> & t_SolarRadiation,
                 const std::shared_ptr<FenestrationCommon::CSeries> & t_DetectorData = nullptr);

        double getPropertySimple(FenestrationCommon::PropertySimple t_Property,
                                 FenestrationCommon::Side t_Side,
                                 FenestrationCommon::Scattering t_Scattering,
                                 double t_Theta = 0,
                                 double t_Phi = 0) override;

        double getMinLambda() const override;
        double getMaxLambda() const override;

        std::vector<double> getWavelengths() const override;

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

        double getAbsorptanceLayer(size_t index,
                                   FenestrationCommon::Side side,
                                   FenestrationCommon::ScatteringSimple scattering,
                                   double theta,
                                   double phi);

        // Absorptances of each layer based on angle of incidence
        double Abs(size_t Index,
                   double t_Angle,
                   double minLambda,
                   double maxLambda,
                   FenestrationCommon::IntegrationType t_IntegrationType =
                     FenestrationCommon::IntegrationType::Trapezoidal,
                   double normalizationCoefficient = 1);

        // Hemispherical absorptances of each layer. Integration is performed over t_Angles.
        double AbsHemispherical(size_t Index,
                                const std::vector<double> & t_IntegrationAngles,
                                double minLambda,
                                double maxLambda,
                                FenestrationCommon::IntegrationType t_IntegrationType =
                                  FenestrationCommon::IntegrationType::Trapezoidal,
                                double normalizationCoefficient = 1);

    private:
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
        std::vector<SingleLayerOptics::SpecularLayer> m_Layers;

        std::vector<double> m_CommonWavelengths;
        std::shared_ptr<FenestrationCommon::CSeries> m_SolarRadiation;
        std::shared_ptr<FenestrationCommon::CSeries> m_DetectorData;

        // Results for angle-properties std::pair. If same angle is required twice, then model will
        // not calculate it twice. First it will search for results here and if results are not
        // available, then it will perform calculation for given angle
        std::vector<CEquivalentLayerSingleComponentMWAngle> m_EquivalentAngle;

        SeriesResults getSeriesResults(const SingleLayerOptics::CBeamDirection & aDirection,
                                       size_t layerIndex);
    };

}   // namespace MultiLayerOptics


#endif
