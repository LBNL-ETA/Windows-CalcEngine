#ifndef MULTIBSDFLAYER_H
#define MULTIBSDFLAYER_H

#include <memory>
#include <vector>
#include <map>
#include <WCECommon.hpp>

#include "EquivalentBSDFLayer.hpp"

namespace FenestrationCommon
{
    class CSeries;
    class SquareMatrix;
    enum class Side;
    enum class PropertySimple;

}   // namespace FenestrationCommon

namespace SingleLayerOptics
{
    class CBSDFIntegrator;
}

namespace MultiLayerOptics
{
    class CEquivalentBSDFLayer;

    typedef std::shared_ptr<FenestrationCommon::CSeries> p_Series;
    typedef std::shared_ptr<std::vector<p_Series>> p_VectorSeries;

    class CMultiPaneBSDF
    {
    public:
        static std::unique_ptr<CMultiPaneBSDF>
          create(const std::shared_ptr<SingleLayerOptics::CBSDFLayer> & t_Layer,
                 const p_Series & t_SolarRadiation,
                 const std::vector<double> & t_CommonWavelengths,
                 const p_VectorSeries & t_IncomingSpectra = nullptr);

        static std::unique_ptr<CMultiPaneBSDF>
          create(const std::shared_ptr<SingleLayerOptics::CBSDFLayer> & t_Layer,
                 const p_Series & t_SolarRadiation,
                 const p_VectorSeries & t_IncomingSpectra = nullptr);

        void setIntegrationType(FenestrationCommon::IntegrationType t_type,
                                double normalizationCoefficient);

        void addLayer(const std::shared_ptr<SingleLayerOptics::CBSDFLayer> & t_Layer);

        // Whole matrix results
        FenestrationCommon::SquareMatrix getMatrix(double minLambda,
                                                   double maxLambda,
                                                   FenestrationCommon::Side t_Side,
                                                   FenestrationCommon::PropertySimple t_Property);

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
        std::shared_ptr<std::vector<double>>
          Abs(double minLambda, double maxLambda, FenestrationCommon::Side t_Side, size_t Index);

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
	private:

		// t_SolarRadiation is spectra used for initialization of material properties in the layers
		// t_IncomingSpectra is solar radiation distribution used to calculate actual data.
		// If t_IncomingSpectra is missing then t_SolarRadiation is considered to be incoming
		// spectra for every direction
		CMultiPaneBSDF( const std::shared_ptr< SingleLayerOptics::CBSDFLayer > & t_Layer,
						const p_Series & t_SolarRadiation,
						const std::vector< double > & t_CommonWavelengths,
						const p_VectorSeries & t_IncomingSpectra = nullptr );

		CMultiPaneBSDF(const std::shared_ptr<SingleLayerOptics::CBSDFLayer> & t_Layer,
					   const p_Series & t_SolarRadiation,
					   const p_VectorSeries & t_IncomingSpectra = nullptr);

        void calculate(double minLambda, double maxLambda);

        void calcHemisphericalAbs(FenestrationCommon::Side t_Side);

        CEquivalentBSDFLayer m_Layer;

        // Solar radiation for initialization
        std::shared_ptr<FenestrationCommon::CSeries> m_SolarRadiationInit;

        p_VectorSeries m_IncomingSpectra;
        std::vector<double> m_IncomingSolar;

        std::shared_ptr<SingleLayerOptics::CBSDFIntegrator> m_Results;

        std::map<FenestrationCommon::Side,
                 std::shared_ptr<std::vector<std::shared_ptr<std::vector<double>>>>>
          m_Abs;

        // Hemispherical absorptances for every layer
        std::map<FenestrationCommon::Side, std::shared_ptr<std::vector<double>>> m_AbsHem;

        bool m_Calculated;
        double m_MinLambdaCalculated;
        double m_MaxLambdaCalculated;

        FenestrationCommon::IntegrationType m_Integrator;
        double m_NormalizationCoefficient;
    };

}   // namespace MultiLayerOptics

#endif
