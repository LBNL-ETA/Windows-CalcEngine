#ifndef EQUIVALENTBSDFLAYERMULTIWL_H
#define EQUIVALENTBSDFLAYERMULTIWL_H

#include <memory>
#include <vector>
#include <map>

#include <WCECommon.hpp>
#include <WCESingleLayerOptics.hpp>

namespace MultiLayerOptics
{
    class CEquivalentBSDFLayerSingleBand;

    // Calculates equivalent BSDF matrices for transmittances and reflectances and vectors for
    // absorptances
    class CEquivalentBSDFLayer
    {
    public:
        explicit CEquivalentBSDFLayer(const std::vector<double> & t_CommonWavelengths);


        explicit CEquivalentBSDFLayer(
          std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> t_Layer,
          const std::optional<std::vector<double>> & matrixWavelengths);

        void addLayer(const std::shared_ptr<SingleLayerOptics::CBSDFLayer> & t_Layer);
        [[nodiscard]] const SingleLayerOptics::BSDFDirections &
          getDirections(SingleLayerOptics::BSDFDirection t_Side) const;

        [[nodiscard]] std::vector<double> getCommonWavelengths() const;
        [[nodiscard]] double getMinLambda() const;
        [[nodiscard]] double getMaxLambda() const;

        // Absorptance wavelength by wavelength matrices
        FenestrationCommon::CMatrixSeries getTotalA(const FenestrationCommon::Side t_Side);

        // Photovoltaic current (scaled to income irradiance equal to one)
        FenestrationCommon::CMatrixSeries getTotalJSC(FenestrationCommon::Side t_Side);

        // Transmittance and reflectance wavelength by wavelength matrices
        FenestrationCommon::CMatrixSeries
          getTotal(const FenestrationCommon::Side t_Side,
                   const FenestrationCommon::PropertySimple t_Property);

        void setSolarRadiation(FenestrationCommon::CSeries & t_SolarRadiation);

        [[nodiscard]] std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & getLayers();
        [[nodiscard]] size_t numberOfLayers() const;

        void setMatrixLayerWavelengths(const std::vector<double> & wavelenghts);

    private:
        void calculate();

        // Wavelength layer per layer calculations
        void calculateWavelengthProperties(size_t t_NumOfLayers, const std::vector<double> & wavelengths);

        void updateWavelengthLayers(SingleLayerOptics::CBSDFLayer & t_Layer);

        static std::vector<double> unionOfLayerWavelengths(
          const std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & t_Layer);

        // std::vector of layer results over each wavelength
        std::vector<CEquivalentBSDFLayerSingleBand> m_LayersWL;

        // Layers that are added to the equivalent layer
        std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> m_Layer;

        // Total absorptance coefficients for every wavelength (does not include source data)
        // First dimension contain number of layers and second dimension is BSDF matrix size
        // Each series contain the data arranged by wavelengths (absorptance for the given
        // wavelength)
        std::map<FenestrationCommon::Side, FenestrationCommon::CMatrixSeries> m_TotA;

        // Total photovoltaic current (see above comment for absorptance
        std::map<FenestrationCommon::Side, FenestrationCommon::CMatrixSeries> m_TotJSC;

        // Total Transmittance and Reflectance values for every wavelength (does not include source
        // data)
        std::map<std::pair<FenestrationCommon::Side, FenestrationCommon::PropertySimple>,
                 FenestrationCommon::CMatrixSeries>
          m_Tot;

        FenestrationCommon::SquareMatrix m_Lambda;

        std::vector<double> m_CombinedLayerWavelengths;
        bool m_Calculated;
    };

}   // namespace MultiLayerOptics

#endif
