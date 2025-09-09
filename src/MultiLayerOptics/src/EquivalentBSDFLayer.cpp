#include "EquivalentBSDFLayer.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"

using FenestrationCommon::CMatrixSeries;
using FenestrationCommon::Side;
using FenestrationCommon::PropertySurface;
using FenestrationCommon::CSeries;

namespace MultiLayerOptics
{
    CEquivalentBSDFLayer::CEquivalentBSDFLayer(const std::vector<double> & t_CommonWavelengths) :
        m_CombinedLayerWavelengths(t_CommonWavelengths)
    {}

    CEquivalentBSDFLayer::CEquivalentBSDFLayer(
      std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> t_Layer,
      const std::optional<std::vector<double>> & matrixWavelengths) :
        m_Layer(std::move(t_Layer)),
        m_Lambda(m_Layer[0]->getResults().lambdaMatrix()),
        m_CombinedLayerWavelengths(matrixWavelengths.has_value()
                                     ? matrixWavelengths.value()
                                     : unionOfLayerWavelengths(m_Layer))
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
        ensureCache();
        return m_TotA.at(t_Side);
    }

    CMatrixSeries CEquivalentBSDFLayer::getTotalJSC(Side t_Side)
    {
        ensureCache();
        return m_TotJSC.at(t_Side);
    }

    CMatrixSeries CEquivalentBSDFLayer::getTotal(const Side t_Side,
                                                 const PropertySurface t_Property)
    {
        ensureCache();
        return m_Tot.at({t_Side, t_Property});
    }

    std::vector<FenestrationCommon::MatrixAtWavelength>
      CEquivalentBSDFLayer::getWavelengthMatrices(Side t_Side, PropertySurface t_Property)
    {
        ensureCache();
        return m_Tot.at({t_Side, t_Property}).seriesMatrices();
    }

    void CEquivalentBSDFLayer::setSolarRadiation(CSeries & t_SolarRadiation)
    {
        // Need to recreate wavelength by wavelength layers
        for(auto & aLayer : m_Layer)
        {
            aLayer->setSourceData(t_SolarRadiation);
        }
        invalidateCache();
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
    }

    bool CEquivalentBSDFLayer::hasCache() const
    {
        return !m_Tot.empty();
    }

    void CEquivalentBSDFLayer::ensureCache()
    {
        if(!hasCache())
        {
            calculate();
        }
    }

    void CEquivalentBSDFLayer::invalidateCache()
    {
        m_Tot.clear();
        m_TotA.clear();
        m_TotJSC.clear();
    }

    void CEquivalentBSDFLayer::calculateWavelengthByWavelengthProperties(
      const FenestrationCommon::ProgressCallback & callback)
    {
        using FenestrationCommon::Side;

        // 0) Precompute Jsc' for ALL layers, both sides, across ALL wavelengths (once)
        std::vector<std::vector<std::vector<double>>> jscFront(m_Layer.size());
        std::vector<std::vector<std::vector<double>>> jscBack(m_Layer.size());
        for(size_t i = 0; i < m_Layer.size(); ++i)
        {
            jscFront[i] = m_Layer[i]->jscPrime(Side::Front, m_CombinedLayerWavelengths);
            jscBack[i] = m_Layer[i]->jscPrime(Side::Back, m_CombinedLayerWavelengths);
        }

        FenestrationCommon::executeInParallel<size_t>(
          0u,
          m_CombinedLayerWavelengths.size() - 1u,
          [this, &jscFront, &jscBack](size_t index) {
              // 1) Build the per-wavelength equivalent layer using the precomputed slices
              auto wl0 = m_Layer[0]->getResultsAtWavelength(index);
              CEquivalentBSDFLayerSingleBand layer{wl0, jscFront[0][index], jscBack[0][index]};
              for(size_t i = 1; i < m_Layer.size(); ++i)
              {
                  auto wli = m_Layer[i]->getResultsAtWavelength(index);
                  layer.addLayer(wli, jscFront[i][index], jscBack[i][index]);
              }

              // 2) Store results (unchanged)
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
        using FenestrationCommon::Side;

        // First layer
        auto jscPrimeFront = m_Layer[0]->jscPrime(Side::Front, m_CombinedLayerWavelengths);
        auto jscPrimeBack = m_Layer[0]->jscPrime(Side::Back, m_CombinedLayerWavelengths);
        auto wl0 = m_Layer[0]->getResultsAtWavelength(wavelengthIndex);

        CEquivalentBSDFLayerSingleBand result{
          wl0, jscPrimeFront[wavelengthIndex], jscPrimeBack[wavelengthIndex]};

        // Remaining layers
        for(size_t i = 1; i < m_Layer.size(); ++i)
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
