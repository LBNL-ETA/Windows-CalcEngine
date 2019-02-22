#ifndef MultiPaneSpecular_H
#define MultiPaneSpecular_H

#include <memory>
#include <vector>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"

namespace FenestrationCommon
{
    enum class Side;
    enum class Property;

    class CSeries;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    class CSpecularCell;
    class SpecularLayer;

}   // namespace SingleLayerOptics

namespace MultiLayerOptics
{
    class CEquivalentLayerSingleComponentMW;

    class CAbsorptancesMultiPane;

    class CEquivalentLayerSingleComponentMWAngle
    {
    public:
        CEquivalentLayerSingleComponentMWAngle(
          const std::shared_ptr<CEquivalentLayerSingleComponentMW> & t_Layer,
          const std::shared_ptr<CAbsorptancesMultiPane> & t_Abs,
          double t_Angle);
        double angle() const;
        std::shared_ptr<CEquivalentLayerSingleComponentMW> layer() const;
        std::shared_ptr<FenestrationCommon::CSeries>
          getProperties(FenestrationCommon::Side t_Side, FenestrationCommon::Property t_Property);
        std::shared_ptr<FenestrationCommon::CSeries> Abs(size_t Index);

    private:
        std::shared_ptr<CEquivalentLayerSingleComponentMW> m_Layer;
        std::shared_ptr<CAbsorptancesMultiPane> m_Abs;
        double m_Angle;
    };

    // Handles equivalent properties of MultiLayerOptics glass consists only of specular layers
    class CMultiPaneSpecular : public SingleLayerOptics::IScatteringLayer
    {
    private:
        CMultiPaneSpecular(std::vector<SingleLayerOptics::SpecularLayer> layers,
                           const std::shared_ptr<FenestrationCommon::CSeries> & t_SolarRadiation);

        CMultiPaneSpecular(std::vector<double> const & t_CommonWavelength,
                           const std::shared_ptr<FenestrationCommon::CSeries> & t_SolarRadiation,
                           SingleLayerOptics::SpecularLayer & t_Layer);

        void addLayer(SingleLayerOptics::SpecularLayer & t_Layer);

    public:
        static std::unique_ptr<CMultiPaneSpecular>
          create(std::vector<SingleLayerOptics::SpecularLayer> layers,
                 const std::shared_ptr<FenestrationCommon::CSeries> & t_SolarRadiation);

        double getPropertySimple(FenestrationCommon::PropertySimple t_Property,
                                 FenestrationCommon::Side t_Side,
                                 FenestrationCommon::Scattering t_Scattering,
                                 double t_Theta,
                                 double t_Phi) override;

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
                                        const std::shared_ptr<const std::vector<double>> & t_Angles,
                                        double minLambda,
                                        double maxLambda,
                                        FenestrationCommon::IntegrationType t_IntegrationType =
                                          FenestrationCommon::IntegrationType::Trapezoidal,
                                        double normalizationCoefficient = 1);

        double getAbsorptanceLayer(size_t index, FenestrationCommon::Side side,
            FenestrationCommon::ScatteringSimple scattering, double theta, double phi);

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
                                const std::shared_ptr<const std::vector<double>> & t_Angles,
                                double minLambda,
                                double maxLambda,
                                FenestrationCommon::IntegrationType t_IntegrationType =
                                  FenestrationCommon::IntegrationType::Trapezoidal,
                                double normalizationCoefficient = 1);

    private:
        // Get correct angular object out of array and if object does not exists, then it just
        // creates new one and stores it into array
        std::shared_ptr<CEquivalentLayerSingleComponentMWAngle> getAngular(double t_Angle);
        // creates equivalent layer properties for certain angle
        std::shared_ptr<CEquivalentLayerSingleComponentMWAngle> createNewAngular(double t_Angle);

        // Contains all specular layers (cells) that are added to the model. This way program will
        // be able to recalculate equivalent properties for any angle
        std::vector<SingleLayerOptics::SpecularLayer> m_Layers;

        std::vector<double> m_CommonWavelengths;
        std::shared_ptr<FenestrationCommon::CSeries> m_SolarRadiation;

        // Results for angle-properties std::pair. If same angle is required twice, then model will
        // not calculate it twice. First it will search for results here and if results are not
        // available, then it will perform calculation for given angle
        std::vector<std::shared_ptr<CEquivalentLayerSingleComponentMWAngle>> m_EquivalentAngle;
    };

}   // namespace MultiLayerOptics


#endif
