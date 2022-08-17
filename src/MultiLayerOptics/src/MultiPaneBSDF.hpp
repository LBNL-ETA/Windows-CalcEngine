#pragma once

#include <memory>
#include <vector>
#include <map>
#include <WCECommon.hpp>
#include <WCESingleLayerOptics.hpp>

#include "EquivalentBSDFLayer.hpp"

namespace MultiLayerOptics
{
    class CEquivalentBSDFLayer;

    class CMultiPaneBSDF : public SingleLayerOptics::IScatteringLayer
    {
    public:
        static std::unique_ptr<CMultiPaneBSDF>
          create(const std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & t_Layer,
                 const std::optional<std::vector<double>> & matrixWavelengths = std::nullopt);

        void setIntegrationType(FenestrationCommon::IntegrationType t_type,
                                double normalizationCoefficient);

        // Whole matrix results
        FenestrationCommon::SquareMatrix getMatrix(double minLambda,
                                                   double maxLambda,
                                                   FenestrationCommon::Side t_Side,
                                                   FenestrationCommon::PropertySimple t_Property);

        double getPropertySimple(const double minLambda,
                                 const double maxLambda,
                                 const FenestrationCommon::PropertySimple t_Property,
                                 const FenestrationCommon::Side t_Side,
                                 const FenestrationCommon::Scattering t_Scattering,
                                 const double t_Theta = 0,
                                 const double t_Phi = 0) override;

        double DirDir(double minLambda,
                      double maxLambda,
                      FenestrationCommon::Side t_Side,
                      FenestrationCommon::PropertySimple t_Property,
                      double t_Theta,
                      double t_Phi);

        double DirDir(double minLambda,
                      double maxLambda,
                      FenestrationCommon::Side t_Side,
                      FenestrationCommon::PropertySimple t_Property,
                      size_t Index);

        // std::vector of layer by layer absorptances for each incoming direction
        std::vector<double> &
          Abs(double minLambda, double maxLambda, FenestrationCommon::Side t_Side, size_t Index);

        std::vector<double> AbsHeat(double minLambda,
                                    double maxLambda,
                                    FenestrationCommon::Side t_Side,
                                    size_t Index);

        std::vector<double> & AbsElectricity(double minLambda,
                                             double maxLambda,
                                             FenestrationCommon::Side t_Side,
                                             size_t Index);

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

        // Hemispherical results for every direction
        std::vector<double> DirHem(double minLambda,
                                   double maxLambda,
                                   FenestrationCommon::Side t_Side,
                                   FenestrationCommon::PropertySimple t_Property);

        // Directional hemispherical results for given Theta and Phi direction
        double DirHem(double minLambda,
                      double maxLambda,
                      FenestrationCommon::Side t_Side,
                      FenestrationCommon::PropertySimple t_Property,
                      double t_Theta,
                      double t_Phi);

        // This function will return portion of property that goes only as diffuse part. In other
        // words, this will be DirHemispherical - DirDir.
        double DirDiff(double minLambda,
                       double maxLambda,
                       FenestrationCommon::Side t_Side,
                       FenestrationCommon::PropertySimple t_Property,
                       double t_Theta,
                       double t_Phi);

        double DirHem(double minLambda,
                      double maxLambda,
                      FenestrationCommon::Side t_Side,
                      FenestrationCommon::PropertySimple t_Property,
                      size_t Index);

        double Abs(double minLambda,
                   double maxLambda,
                   FenestrationCommon::Side t_Side,
                   size_t layerIndex,
                   double t_Theta,
                   double t_Phi);

        double AbsHeat(double minLambda,
                       double maxLambda,
                       FenestrationCommon::Side t_Side,
                       size_t layerIndex,
                       double t_Theta,
                       double t_Phi);

        double AbsElectricity(double minLambda,
                              double maxLambda,
                              FenestrationCommon::Side t_Side,
                              size_t layerIndex,
                              double t_Theta,
                              double t_Phi);

        double Abs(double minLambda,
                   double maxLambda,
                   FenestrationCommon::Side t_Side,
                   size_t layerIndex,
                   size_t beamIndex);

        // Diffuse to diffuse properties
        double DiffDiff(double minLambda,
                        double maxLambda,
                        FenestrationCommon::Side t_Side,
                        FenestrationCommon::PropertySimple t_Property);

        double AbsDiff(double minLambda,
                       double maxLambda,
                       FenestrationCommon::Side t_Side,
                       size_t t_LayerIndex);

        double AbsDiffHeat(double minLambda,
                           double maxLambda,
                           FenestrationCommon::Side t_Side,
                           size_t t_LayerIndex);

        double AbsDiffElectricity(double minLambda,
                                  double maxLambda,
                                  FenestrationCommon::Side t_Side,
                                  size_t t_LayerIndex);

        // Energy that gets transmitted or reflected from certain direction
        double energy(double minLambda,
                      double maxLambda,
                      FenestrationCommon::Side t_Side,
                      FenestrationCommon::PropertySimple t_Property,
                      double t_Theta,
                      double t_Phi);

        double energyAbs(double minLambda,
                         double maxLambda,
                         FenestrationCommon::Side t_Side,
                         size_t Index,
                         double t_Theta,
                         double t_Phi);

        [[nodiscard]] std::vector<double> getWavelengths() const override;

        [[nodiscard]] double getMinLambda() const override;
        [[nodiscard]] double getMaxLambda() const override;

        void setCalculationProperties(const SingleLayerOptics::CalculationProperties & calcProperties) override;

    protected:
        explicit CMultiPaneBSDF(
          const std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & t_Layer,
          const std::optional<std::vector<double>> & matrixWavelengths);

        std::vector<std::vector<double>>
          calcPVLayersElectricity(const std::vector<std::vector<double>> & jsc,
                                  const std::vector<double> & incomingSolar);

        void calculate(double minLambda, double maxLambda);

        void calcHemisphericalAbs(FenestrationCommon::Side t_Side);

        [[nodiscard]] std::vector<double> getCommonWavelengthsFromLayers(
          const std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & t_Layer) const;

        static double integrateBSDFAbsorptance(const std::vector<double> & lambda,
                                               const std::vector<double> & absorptance);

        CEquivalentBSDFLayer m_EquivalentLayer;

        // Solar radiation for initialization
        FenestrationCommon::CSeries m_SolarRadiationInit;

        std::vector<FenestrationCommon::CSeries> m_IncomingSpectra;
        std::vector<double> m_IncomingSolar;

        SingleLayerOptics::BSDFIntegrator m_Results;

        // Absorptances of every layer and every incoming direction in BSDF integrated over given
        // range
        std::map<FenestrationCommon::Side, std::vector<std::vector<double>>> m_Abs;
        std::map<FenestrationCommon::Side, std::vector<std::vector<double>>> m_AbsElectricity;

        // Hemispherical absorptances for every layer
        std::map<FenestrationCommon::Side, std::vector<double>> m_AbsHem;
        std::map<FenestrationCommon::Side, std::vector<double>> m_AbsHemElectricity;

        bool m_Calculated;
        double m_MinLambdaCalculated;
        double m_MaxLambdaCalculated;

        FenestrationCommon::IntegrationType m_Integrator;
        double m_NormalizationCoefficient;

        SingleLayerOptics::BSDFDirections m_BSDFDirections;

        // These are wavelength used only for the spectral integration seprately from wavelengths in
        // matrices calculations. Matrices wavelenghts will be used only if this is not provided.
        std::optional<std::vector<double>> m_SpectralIntegrationWavelengths;
    };

}   // namespace MultiLayerOptics
