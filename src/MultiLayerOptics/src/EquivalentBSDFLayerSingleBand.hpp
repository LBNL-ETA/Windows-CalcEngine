#ifndef EQUIVALENTBSDFLAYERSINGLEBAND_H
#define EQUIVALENTBSDFLAYERSINGLEBAND_H

#include <memory>
#include <vector>
#include <map>

#include "WCECommon.hpp"
#include "../../SingleLayerOptics/src/BSDFIntegrator.hpp"

namespace SingleLayerOptics
{
    class CBSDFIntegrator;
}

namespace MultiLayerOptics
{
    // Matrix will store absorptances for each layer at every direction
    typedef std::vector<std::vector<double>> Abs_Matrix;

    FenestrationCommon::SquareMatrix
      interReflectance(const FenestrationCommon::SquareMatrix & t_Lambda,
                       const FenestrationCommon::SquareMatrix & t_Rb,
                       const FenestrationCommon::SquareMatrix & t_Rf);

    // Class to calculate equivalent BSDF transmittance and reflectances. This will be used by
    // multilayer routines to calculate properties for any number of layers.
    class CBSDFDoubleLayer
    {
    public:
        CBSDFDoubleLayer(const SingleLayerOptics::CBSDFIntegrator & t_FrontLayer,
                         const SingleLayerOptics::CBSDFIntegrator & t_BackLayer);

        [[nodiscard]] std::shared_ptr<SingleLayerOptics::CBSDFIntegrator> value() const;

    private:
        static FenestrationCommon::SquareMatrix
          equivalentT(const FenestrationCommon::SquareMatrix & t_Tf2,
                      const FenestrationCommon::SquareMatrix & t_InterRefl,
                      const FenestrationCommon::SquareMatrix & t_Lambda,
                      const FenestrationCommon::SquareMatrix & t_Tf1);

        static FenestrationCommon::SquareMatrix
          equivalentR(const FenestrationCommon::SquareMatrix & t_Rf1,
                      const FenestrationCommon::SquareMatrix & t_Tf1,
                      const FenestrationCommon::SquareMatrix & t_Tb1,
                      const FenestrationCommon::SquareMatrix & t_Rf2,
                      const FenestrationCommon::SquareMatrix & t_InterRefl,
                      const FenestrationCommon::SquareMatrix & t_Lambda);

        std::shared_ptr<SingleLayerOptics::CBSDFIntegrator> m_Results;

        FenestrationCommon::SquareMatrix m_Tf;
        FenestrationCommon::SquareMatrix m_Tb;
        FenestrationCommon::SquareMatrix m_Rf;
        FenestrationCommon::SquareMatrix m_Rb;
    };

    // Class for equivalent BSDF layer for single material properties (or single wavelength)
    class CEquivalentBSDFLayerSingleBand
    {
    public:
        explicit CEquivalentBSDFLayerSingleBand(
          const std::shared_ptr<SingleLayerOptics::CBSDFIntegrator> & t_Layer,
          const std::vector<double> & jscPrimeFront = std::vector<double>(),
          const std::vector<double> & jscPrimeBack = std::vector<double>());
        void addLayer(const std::shared_ptr<SingleLayerOptics::CBSDFIntegrator> & t_Layer,
                      const std::vector<double> & jcsFront = std::vector<double>(),
                      const std::vector<double> & jcsBack = std::vector<double>());
        void BuildForwardAndBackwardLayers(size_t numberOfLayers);
        void CreateIplusAndIminusValues(size_t numberOfLayers, size_t matrixSize);
        void CalculateLayerAbsorptances(size_t numberOfLayers);

        FenestrationCommon::SquareMatrix getMatrix(FenestrationCommon::Side t_Side,
                                                   FenestrationCommon::PropertySimple t_Property);

        FenestrationCommon::SquareMatrix getProperty(FenestrationCommon::Side t_Side,
                                                     FenestrationCommon::PropertySimple t_Property);

        std::vector<double> getLayerAbsorptances(size_t Index, FenestrationCommon::Side t_Side);
        std::vector<double> getLayerJSC(size_t Index, FenestrationCommon::Side t_Side);

        [[nodiscard]] size_t getNumberOfLayers() const;

    private:
        void calcEquivalentProperties();

        [[nodiscard]] FenestrationCommon::SquareMatrix
          iminusCalc(const FenestrationCommon::SquareMatrix & t_InterRefl,
                     const FenestrationCommon::SquareMatrix & t_T) const;

        [[nodiscard]] FenestrationCommon::SquareMatrix
          iplusCalc(const FenestrationCommon::SquareMatrix & t_InterRefl,
                    const FenestrationCommon::SquareMatrix & t_R,
                    const FenestrationCommon::SquareMatrix & t_T) const;

        std::shared_ptr<SingleLayerOptics::CBSDFIntegrator> m_EquivalentLayer;
        std::vector<std::shared_ptr<SingleLayerOptics::CBSDFIntegrator>> m_Layers;

        // Forward and backward layers are used for calculation of equivalent absorptances
        std::vector<std::shared_ptr<SingleLayerOptics::CBSDFIntegrator>> m_Forward;
        std::vector<std::shared_ptr<SingleLayerOptics::CBSDFIntegrator>> m_Backward;

        // Equations for absorptance calculations are described in "Klems-Matrix Layer Calculations"
        // document. Two equations (3.7a) and (3.7b) are used to calculate front and back
        // absorptances. In to process of calculation incoming and outgoing rays are calculated and
        // stored into this map. Iminus and Iplus are stored in a way that
        // Iminus[EnergyFlow::Forward][i] and Iplus[EnergyFlow::Backward][i] are representing front
        // and back incoming irradinace at the layer on the position "i"
        std::map<FenestrationCommon::EnergyFlow, std::vector<FenestrationCommon::SquareMatrix>>
          m_Iminus;
        std::map<FenestrationCommon::EnergyFlow, std::vector<FenestrationCommon::SquareMatrix>>
          m_Iplus;

        // Photovoltaic properties for every direction. Vector is scaled to incoming
        // irradiance set to one.
        std::map<FenestrationCommon::Side, std::vector<std::vector<double>>> m_JSCPrime{
          {FenestrationCommon::Side::Front, Abs_Matrix()},
          {FenestrationCommon::Side::Back, Abs_Matrix()}};

        // Absorptance is stored for each layer and every direction (Abs_Matrix)
        std::map<FenestrationCommon::Side, Abs_Matrix> m_A;
        std::map<FenestrationCommon::Side, Abs_Matrix> m_JSC;

        bool m_PropertiesCalculated;

        FenestrationCommon::SquareMatrix m_Lambda;
    };

}   // namespace MultiLayerOptics

#endif
