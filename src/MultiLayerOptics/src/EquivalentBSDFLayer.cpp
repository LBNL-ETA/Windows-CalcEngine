
#include <cmath>

#include <thread>

#include "EquivalentBSDFLayer.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"

using FenestrationCommon::CMatrixSeries;
using FenestrationCommon::Side;
using FenestrationCommon::PropertySurface;
using FenestrationCommon::CSeries;

namespace MultiLayerOptics
{
    CEquivalentBSDFLayer::CEquivalentBSDFLayer(const std::vector<double> & t_CommonWavelengths) :
        m_CombinedLayerWavelengths(t_CommonWavelengths), m_Calculated(false)
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
            layer->setBandWavelengths(m_CombinedLayerWavelengths);
        }
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

    CMatrixSeries CEquivalentBSDFLayer::getTotal(const Side t_Side, const PropertySurface t_Property)
    {
        if(!m_Calculated)
        {
            calculate();
        }
        return m_Tot.at({t_Side, t_Property});
    }

    std::vector<FenestrationCommon::MatrixAtWavelength>
      CEquivalentBSDFLayer::getWavelengthMatrices(Side t_Side, PropertySurface t_Property)
    {
        if(!m_Calculated)
        {
            calculate();
        }
        return m_Tot.at({t_Side, t_Property}).seriesMatrices();
    }

    void CEquivalentBSDFLayer::setSolarRadiation(CSeries & t_SolarRadiation)
    {
        // Need to recreate wavelenght by wavelength layers
        for(auto & aLayer : m_Layer)
        {
            aLayer->setSourceData(t_SolarRadiation);
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

    void CEquivalentBSDFLayer::calculate(const FenestrationCommon::ProgressCallback & callback)
    {
        for(Side aSide : FenestrationCommon::allSides())
        {
            m_TotA[aSide] =
              CMatrixSeries(m_Layer.size(), m_Lambda.size(), m_CombinedLayerWavelengths.size());
            m_TotJSC[aSide] =
              CMatrixSeries(m_Layer.size(), m_Lambda.size(), m_CombinedLayerWavelengths.size());
            for(PropertySurface aProperty : FenestrationCommon::allPropertySimple())
            {
                m_Tot[{aSide, aProperty}] = CMatrixSeries(
                  m_Lambda.size(), m_Lambda.size(), m_CombinedLayerWavelengths.size());
            }
        }

        calculateWavelengthByWavelengthProperties(callback);

        m_Calculated = true;
    }

    void CEquivalentBSDFLayer::calculateWavelengthByWavelengthProperties(
      const FenestrationCommon::ProgressCallback & callback)
    {
        FenestrationCommon::executeInParallel<size_t>(
          0u,
          m_CombinedLayerWavelengths.size() - 1u,
          [this](size_t index) {
              // Do not refactor auto layer variable out since calling
              // getEquivalentLayerAtWavelength is calculation intensive and it will slow down the
              // execution
              auto layer{getEquivalentLayerAtWavelength(index)};
              for(auto aSide : FenestrationCommon::allSides())
              {
                  for(size_t layerNumber = 0; layerNumber < m_Layer.size(); ++layerNumber)
                  {
                      m_TotA.at(aSide).setPropertiesAtIndex(
                        index,
                        layerNumber,
                        m_CombinedLayerWavelengths[index],
                        layer.getLayerAbsorptances(layerNumber + 1, aSide));

                      m_TotJSC.at(aSide).setPropertiesAtIndex(
                        index,
                        layerNumber,
                        m_CombinedLayerWavelengths[index],
                        layer.getLayerJSC(layerNumber + 1, aSide));
                  }
                  for(auto aProperty : FenestrationCommon::allPropertySimple())
                  {
                      m_Tot.at({aSide, aProperty})
                        .setPropertiesAtIndex(index,
                                              m_CombinedLayerWavelengths[index],
                                              layer.getProperty(aSide, aProperty));
                  }
              }
          },
          callback);
    }

    CEquivalentBSDFLayerSingleBand
      CEquivalentBSDFLayer::getEquivalentLayerAtWavelength(size_t wavelengthIndex) const
    {
        auto jscPrimeFront{m_Layer[0]->jscPrime(Side::Front, m_CombinedLayerWavelengths)};
        auto jscPrimeBack{m_Layer[0]->jscPrime(Side::Back, m_CombinedLayerWavelengths)};
        auto layerWLResults{m_Layer[0]->getResultsAtWavelength(wavelengthIndex)};


        CEquivalentBSDFLayerSingleBand result{
          layerWLResults, jscPrimeFront[wavelengthIndex], jscPrimeBack[wavelengthIndex]};

        for(size_t i = 1u; i < m_Layer.size(); ++i)
        {
            jscPrimeFront = m_Layer[i]->jscPrime(Side::Front, m_CombinedLayerWavelengths);
            jscPrimeBack = m_Layer[i]->jscPrime(Side::Back, m_CombinedLayerWavelengths);
            result.addLayer(m_Layer[i]->getResultsAtWavelength(wavelengthIndex),
                            jscPrimeFront[wavelengthIndex],
                            jscPrimeBack[wavelengthIndex]);
        }

        return result;
    }

    std::vector<double> CEquivalentBSDFLayer::unionOfLayerWavelengths(
      const std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & t_Layer)
    {
        FenestrationCommon::CCommonWavelengths wl;
        for(const auto & layer : t_Layer)
        {
            wl.addWavelength(layer->getBandWavelengths());
        }

        return wl.getCombinedWavelengths(FenestrationCommon::Combine::Interpolate);
    }
}   // namespace MultiLayerOptics
