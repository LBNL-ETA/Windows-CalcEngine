#pragma once

#include <memory>
#include <vector>
#include <map>

#include "WCECommon.hpp"
#include "WCESingleLayerOptics.hpp"

namespace MultiLayerOptics
{
    // Matrix will store absorptances for each layer at every direction
    typedef std::vector<std::vector<double>> Abs_Matrix;

    FenestrationCommon::SquareMatrix
      interReflectance(const std::vector<double> & t_Lambda,
                       const FenestrationCommon::SquareMatrix & t_Rb,
                       const FenestrationCommon::SquareMatrix & t_Rf);

    // Class to calculate equivalent BSDF transmittance and reflectances. This will be used by
    // multilayer routines to calculate properties for any number of layers.
    class CBSDFDoubleLayer
    {
    public:
        CBSDFDoubleLayer(const SingleLayerOptics::BSDFIntegrator & t_FrontLayer,
                         const SingleLayerOptics::BSDFIntegrator & t_BackLayer);

        [[nodiscard]] SingleLayerOptics::BSDFIntegrator value() const;

    private:
        static FenestrationCommon::SquareMatrix
          equivalentT(const FenestrationCommon::SquareMatrix & t_Tf2,
                      const FenestrationCommon::SquareMatrix & t_InterRefl,
                      const std::vector<double> & t_Lambda,
                      const FenestrationCommon::SquareMatrix & t_Tf1);

        static FenestrationCommon::SquareMatrix
          equivalentR(const FenestrationCommon::SquareMatrix & t_Rf1,
                      const FenestrationCommon::SquareMatrix & t_Tf1,
                      const FenestrationCommon::SquareMatrix & t_Tb1,
                      const FenestrationCommon::SquareMatrix & t_Rf2,
                      const FenestrationCommon::SquareMatrix & t_InterRefl,
                      const std::vector<double> & t_Lambda);

        SingleLayerOptics::BSDFIntegrator m_Results;

        FenestrationCommon::SquareMatrix m_Tf;
        FenestrationCommon::SquareMatrix m_Tb;
        FenestrationCommon::SquareMatrix m_Rf;
        FenestrationCommon::SquareMatrix m_Rb;
    };

    // Class for equivalent BSDF layer for single material properties (or single wavelength)
    class CEquivalentBSDFLayerSingleBand
    {
    public:
        CEquivalentBSDFLayerSingleBand() = default;
        explicit CEquivalentBSDFLayerSingleBand(
          const SingleLayerOptics::BSDFIntegrator & t_Layer,
          const std::vector<double> & jscPrimeFront = std::vector<double>(),
          const std::vector<double> & jscPrimeBack = std::vector<double>());
        void addLayer(const SingleLayerOptics::BSDFIntegrator & t_Layer,
                      const std::vector<double> & jcsFront = std::vector<double>(),
                      const std::vector<double> & jcsBack = std::vector<double>());

        FenestrationCommon::SquareMatrix getMatrix(FenestrationCommon::Side t_Side,
                                                   FenestrationCommon::PropertySurface t_Property);

        FenestrationCommon::SquareMatrix getProperty(FenestrationCommon::Side t_Side,
                                                     FenestrationCommon::PropertySurface t_Property);

        std::vector<double> getLayerAbsorptances(size_t Index, FenestrationCommon::Side t_Side);
        std::vector<double> getLayerJSC(size_t Index, FenestrationCommon::Side t_Side);

        [[nodiscard]] size_t getNumberOfLayers() const;

    private:
        // Forward and backward layers are used for calculation of equivalent absorptances
        struct AbsorptanceLayers
        {
            std::vector<SingleLayerOptics::BSDFIntegrator> Forward;
            std::vector<SingleLayerOptics::BSDFIntegrator> Backward;
        };

        AbsorptanceLayers BuildForwardAndBackwardLayers(size_t numberOfLayers);

        void calcEquivalentProperties();

        [[nodiscard]] FenestrationCommon::SquareMatrix
          iminusCalc(const FenestrationCommon::SquareMatrix & t_InterRefl,
                     const FenestrationCommon::SquareMatrix & t_T) const;

        [[nodiscard]] FenestrationCommon::SquareMatrix
          iplusCalc(const FenestrationCommon::SquareMatrix & t_InterRefl,
                    const FenestrationCommon::SquareMatrix & t_R,
                    const FenestrationCommon::SquareMatrix & t_T) const;

        SingleLayerOptics::BSDFIntegrator m_EquivalentLayer;
        std::vector<SingleLayerOptics::BSDFIntegrator> m_Layers;

        struct IrradiationMatrices
        {
            // Equations for absorptance calculations are described in "Klems-Matrix Layer
            // Calculations" document. Two equations (3.7a) and (3.7b) are used to calculate front
            // and back absorptances. In to process of calculation incoming and outgoing rays are
            // calculated and stored into this map. Iminus and Iplus are stored in a way that
            // Iminus[EnergyFlow::Forward][i] and Iplus[EnergyFlow::Backward][i] are representing
            // front and back incoming irradinace at the layer on the position "i"
            std::map<FenestrationCommon::EnergyFlow, std::vector<FenestrationCommon::SquareMatrix>>
              Iminus;
            std::map<FenestrationCommon::EnergyFlow, std::vector<FenestrationCommon::SquareMatrix>>
              Iplus;
        };

        IrradiationMatrices CreateIplusAndIminusValues(size_t numberOfLayers,
                                                       size_t matrixSize,
                                                       AbsorptanceLayers & absLayers);

        void CalculateLayerAbsorptances(size_t numberOfLayers,
                                        IrradiationMatrices irradiation);


        // Photovoltaic properties for every direction. Vector is scaled to incoming
        // irradiance set to one.
        std::map<FenestrationCommon::Side, std::vector<std::vector<double>>> m_JSCPrime{
          {FenestrationCommon::Side::Front, Abs_Matrix()},
          {FenestrationCommon::Side::Back, Abs_Matrix()}};

        // Absorptance is stored for each layer and every direction (Abs_Matrix)
        std::map<FenestrationCommon::Side, Abs_Matrix> m_A;
        std::map<FenestrationCommon::Side, Abs_Matrix> m_JSC;

        bool m_PropertiesCalculated;

        std::vector<double> m_Lambda;
    };

}   // namespace MultiLayerOptics
