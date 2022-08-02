
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <execution>

#include "EquivalentBSDFLayer.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

namespace MultiLayerOptics
{
    CEquivalentBSDFLayer::CEquivalentBSDFLayer(const std::vector<double> & t_CommonWavelengths) :
        m_CombinedLayerWavelengths(t_CommonWavelengths),
        m_Calculated(false)
    {}

    CEquivalentBSDFLayer::CEquivalentBSDFLayer(
      std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> t_Layer,
      const std::optional<std::vector<double>> & matrixWavelengths) :
        m_Layer(std::move(t_Layer)),
        m_Lambda(m_Layer[0]->getResults().lambdaMatrix()),
        m_CombinedLayerWavelengths(matrixWavelengths.has_value()
                                     ? matrixWavelengths.value()
                                     : unionOfLayerWavelengths(m_Layer)),
        m_Calculated(false)
    {
        for(const auto & layer : m_Layer)
        {
            if(matrixWavelengths.has_value())
            {
                layer->setBandWavelengths(matrixWavelengths.value());
            }
            updateWavelengthLayers(*layer);
        }
    }

    void
      CEquivalentBSDFLayer::addLayer(const std::shared_ptr<SingleLayerOptics::CBSDFLayer> & t_Layer)
    {
        t_Layer->setBandWavelengths(m_CombinedLayerWavelengths);
        if(m_Layer.empty())
        {
            m_Lambda = t_Layer->getResults().lambdaMatrix();
        }
        updateWavelengthLayers(*t_Layer);
        m_Layer.push_back(t_Layer);
    }

    const SingleLayerOptics::BSDFDirections &
      CEquivalentBSDFLayer::getDirections(const SingleLayerOptics::BSDFDirection t_Side) const
    {
        return m_Layer[0]->getDirections(t_Side);
    }

    std::vector<double> CEquivalentBSDFLayer::getCommonWavelengths() const
    {
        return m_CombinedLayerWavelengths;
    }

    double CEquivalentBSDFLayer::getMinLambda() const
    {
        return m_CombinedLayerWavelengths.front();
    }

    double CEquivalentBSDFLayer::getMaxLambda() const
    {
        return m_CombinedLayerWavelengths.back();
    }

    CMatrixSeries CEquivalentBSDFLayer::getTotalA(const Side t_Side)
    {
        if(!m_Calculated)
        {
            calculate();
        }
        return m_TotA.at(t_Side);
    }

    CMatrixSeries CEquivalentBSDFLayer::getTotalJSC(Side t_Side)
    {
        if(!m_Calculated)
        {
            calculate();
        }
        return m_TotJSC.at(t_Side);
    }

    CMatrixSeries CEquivalentBSDFLayer::getTotal(const Side t_Side, const PropertySimple t_Property)
    {
        if(!m_Calculated)
        {
            calculate();
        }
        return m_Tot.at({t_Side, t_Property});
    }

    void CEquivalentBSDFLayer::setSolarRadiation(CSeries & t_SolarRadiation)
    {
        // Need to recreate wavelenght by wavelength layers
        m_LayersWL.clear();
        for(auto & aLayer : m_Layer)
        {
            aLayer->setSourceData(t_SolarRadiation);
            updateWavelengthLayers(*aLayer);
        }
        m_Calculated = false;
    }

    std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & CEquivalentBSDFLayer::getLayers()
    {
        return m_Layer;
    }

    size_t CEquivalentBSDFLayer::numberOfLayers() const
    {
        return m_Layer.size();
    }

    void CEquivalentBSDFLayer::setMatrixLayerWavelengths(const std::vector<double> & wavelenghts)
    {
        m_CombinedLayerWavelengths = wavelenghts;
        m_LayersWL.clear();
        for(const auto & layer : m_Layer)
        {
            layer->setBandWavelengths(wavelenghts);
            updateWavelengthLayers(*layer);
        }
    }

    void CEquivalentBSDFLayer::calculate()
    {
        const size_t matrixSize = m_Lambda.size();
        const size_t numberOfLayers = m_LayersWL[0].getNumberOfLayers();

        for(Side aSide : EnumSide())
        {
            m_TotA[aSide] = CMatrixSeries(numberOfLayers, matrixSize);
            m_TotJSC[aSide] = CMatrixSeries(numberOfLayers, matrixSize);
            for(PropertySimple aProperty : EnumPropertySimple())
            {
                m_Tot[{aSide, aProperty}] = CMatrixSeries(matrixSize, matrixSize);
            }
        }

        calculateAndStoreWavelengthProperties(numberOfLayers, m_CombinedLayerWavelengths);

        m_Calculated = true;
    }

    void CEquivalentBSDFLayer::calculateAndStoreWavelengthProperties(
      size_t const t_NumOfLayers, const std::vector<double> & wavelengths)
    {
        auto wlData = createWavelengthByWavelengthData(wavelengths);

        calculateWavelengthByWavelengthProperties(t_NumOfLayers, wlData);

        storeWavelengthByWavelengthProperties(t_NumOfLayers, wlData);
    }

    std::vector<CEquivalentBSDFLayer::wavelenghtData>
      CEquivalentBSDFLayer::createWavelengthByWavelengthData(
        const std::vector<double> & wavelengths)
    {
        std::vector<wavelenghtData> wlData;

        for(size_t i = 0u; i < wavelengths.size(); ++i)
        {
            wlData.emplace_back(wavelengths[i], m_LayersWL[i]);
        }
        return wlData;
    }

    void CEquivalentBSDFLayer::calculateWavelengthByWavelengthProperties(
      const size_t t_NumOfLayers, std::vector<wavelenghtData> & wlData) const
    {
        std::for_each(
          std::execution::par_unseq, wlData.begin(), wlData.end(), [&](wavelenghtData & val) {
              for(auto aSide : EnumSide())
              {
                  for(size_t layerNumber = 0; layerNumber < t_NumOfLayers; ++layerNumber)
                  {
                      val.totA[{aSide, layerNumber}] =
                        val.layer.getLayerAbsorptances(layerNumber + 1, aSide);
                      val.totJSC[{aSide, layerNumber}] =
                        val.layer.getLayerJSC(layerNumber + 1, aSide);
                  }
                  for(auto aProperty : EnumPropertySimple())
                  {
                      val.tot[{aSide, aProperty}] = val.layer.getProperty(aSide, aProperty);
                  }
              }
          });
    }

    void CEquivalentBSDFLayer::storeWavelengthByWavelengthProperties(
      const size_t t_NumOfLayers, const std::vector<wavelenghtData> & wlData)
    {
        for(auto & t : wlData)
        {
            for(auto aSide : EnumSide())
            {
                for(size_t layerNumber = 0; layerNumber < t_NumOfLayers; ++layerNumber)
                {
                    m_TotA.at(aSide).addProperties(
                      layerNumber, t.wavelength, t.totA.at({aSide, layerNumber}));
                    m_TotJSC.at(aSide).addProperties(
                      layerNumber, t.wavelength, t.totJSC.at({aSide, layerNumber}));
                }
                for(auto aProperty : EnumPropertySimple())
                {
                    m_Tot.at({aSide, aProperty})
                      .addProperties(t.wavelength, t.tot.at({aSide, aProperty}));
                }
            }
        }
    }

    void CEquivalentBSDFLayer::updateWavelengthLayers(SingleLayerOptics::CBSDFLayer & t_Layer)
    {
        const auto aResults = t_Layer.getWavelengthResults();
        const auto size = m_CombinedLayerWavelengths.size();

        auto jscPrimeFront{t_Layer.jscPrime(Side::Front, m_CombinedLayerWavelengths)};
        const auto jscPrimeBack{t_Layer.jscPrime(Side::Back, m_CombinedLayerWavelengths)};

        for(size_t i = 0; i < size; ++i)
        {
            const auto curWL = m_CombinedLayerWavelengths[i];
            const auto index = t_Layer.getBandIndex(curWL);
            assert(index > -1);

            const SingleLayerOptics::BSDFIntegrator currentLayer =
              aResults[static_cast<size_t>(index)];

            if(m_LayersWL.size() <= i)
            {
                m_LayersWL.emplace_back(currentLayer, jscPrimeFront[i], jscPrimeBack[i]);
            }
            else
            {
                m_LayersWL[i].addLayer(currentLayer, jscPrimeFront[i], jscPrimeBack[i]);
            }
        }
    }

    std::vector<double> CEquivalentBSDFLayer::unionOfLayerWavelengths(
      const std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & t_Layer)
    {
        CCommonWavelengths wl;
        for(const auto & layer : t_Layer)
        {
            wl.addWavelength(layer->getBandWavelengths());
        }

        return wl.getCombinedWavelengths(Combine::Interpolate);
    }
}   // namespace MultiLayerOptics
