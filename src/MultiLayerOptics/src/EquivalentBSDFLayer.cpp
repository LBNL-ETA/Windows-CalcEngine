
#include <cmath>

#include <thread>

#include "EquivalentBSDFLayer.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"

using FenestrationCommon::CMatrixSeries;
using FenestrationCommon::Side;
using FenestrationCommon::PropertySimple;
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
        FenestrationCommon::logMsg("begin CEquivalentBSDFLayer::CEquivalentBSDFLayer");
        for(const auto & layer : m_Layer)
        {
            FenestrationCommon::logMsg("in for(const auto & layer : m_Layer)");
            layer->setBandWavelengths(m_CombinedLayerWavelengths);
        }
        FenestrationCommon::logMsg("end CEquivalentBSDFLayer::CEquivalentBSDFLayer");
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

    void CEquivalentBSDFLayer::calculate()
    {
        FenestrationCommon::logMsg("begin CEquivalentBSDFLayer::calculate");
        for(Side aSide : FenestrationCommon::EnumSide())
        {
            FenestrationCommon::logMsg("in for(Side aSide : FenestrationCommon::EnumSide())");
            m_TotA[aSide] =
              CMatrixSeries(m_Layer.size(), m_Lambda.size(), m_CombinedLayerWavelengths.size());
            FenestrationCommon::logMsg("before m_TotJSC[aSide] =");
            m_TotJSC[aSide] =
              CMatrixSeries(m_Layer.size(), m_Lambda.size(), m_CombinedLayerWavelengths.size());
            FenestrationCommon::logMsg("before for(PropertySimple");
            for(PropertySimple aProperty : FenestrationCommon::EnumPropertySimple())
            {
                FenestrationCommon::logMsg("in for(PropertySimple aProperty : FenestrationCommon::EnumPropertySimple())");
                m_Tot[{aSide, aProperty}] = CMatrixSeries(
                  m_Lambda.size(), m_Lambda.size(), m_CombinedLayerWavelengths.size());
            }
        }

        FenestrationCommon::logMsg("before CEquivalentBSDFLayer::calculate");
        calculateWavelengthByWavelengthProperties();
        FenestrationCommon::logMsg("before m_Calculated = true");
        m_Calculated = true;
        FenestrationCommon::logMsg("end CEquivalentBSDFLayer::calculate");
    }

    void CEquivalentBSDFLayer::calculateWavelengthByWavelengthProperties()
    {
        FenestrationCommon::logMsg(
          "begin CEquivalentBSDFLayer::calculateWavelengthByWavelengthProperties() with m_CombinedLayerWavelengths.size() = " + std::to_string(m_CombinedLayerWavelengths.size()));
        FenestrationCommon::executeInParallel<size_t>(
          0u, m_CombinedLayerWavelengths.size() - 1u, [this](size_t index) {
              // Do not refactor auto layer variable out since calling
              // getEquivalentLayerAtWavelength is calculation intensive and it will slow down the
              // execution
              auto layer{getEquivalentLayerAtWavelength(index)};
              for(auto aSide : FenestrationCommon::EnumSide())
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
                  for(auto aProperty : FenestrationCommon::EnumPropertySimple())
                  {
                      m_Tot.at({aSide, aProperty})
                        .setPropertiesAtIndex(index,
                                              m_CombinedLayerWavelengths[index],
                                              layer.getProperty(aSide, aProperty));
                  }
              }
          });
        FenestrationCommon::logMsg(
          "end CEquivalentBSDFLayer::calculateWavelengthByWavelengthProperties() with "
          "m_CombinedLayerWavelengths.size() = "
          + std::to_string(m_CombinedLayerWavelengths.size()));

    }

    CEquivalentBSDFLayerSingleBand
      CEquivalentBSDFLayer::getEquivalentLayerAtWavelength(size_t wavelengthIndex) const
    {
        FenestrationCommon::logMsg(
          "begin CEquivalentBSDFLayer::getEquivalentLayerAtWavelength with wavelengthIndex = "
          + std::to_string(wavelengthIndex)
          + " and m_Layer.size() = " + std::to_string(m_Layer.size()));
        auto jscPrimeFront{m_Layer[0]->jscPrime(Side::Front, m_CombinedLayerWavelengths)};
        FenestrationCommon::logMsg("before auto jscPrimeBack");
        auto jscPrimeBack{m_Layer[0]->jscPrime(Side::Back, m_CombinedLayerWavelengths)};
        FenestrationCommon::logMsg("before auto layerWLResults");
        auto layerWLResults{m_Layer[0]->getResultsAtWavelength(wavelengthIndex)};

        FenestrationCommon::logMsg("before CEquivalentBSDFLayerSingleBand result with "
                                   "jscPrimeFront[wavelengthIndex].size() = "
                                   + std::to_string(jscPrimeFront[wavelengthIndex].size())
        +                          + " jscPrimeBack[wavelengthIndex].size() = "
                                   + std::to_string(jscPrimeBack[wavelengthIndex].size()));
        CEquivalentBSDFLayerSingleBand result{
          layerWLResults, jscPrimeFront[wavelengthIndex], jscPrimeBack[wavelengthIndex]};

        for(size_t i = 1u; i < m_Layer.size(); ++i)
        {
            FenestrationCommon::logMsg("in for(size_t i = 1u; i < m_Layer.size(); ++i) with i = "
                                       + std::to_string(i));
            jscPrimeFront = m_Layer[i]->jscPrime(Side::Front, m_CombinedLayerWavelengths);
            FenestrationCommon::logMsg("before jscPrimeBack = m_Layer[i]");
            jscPrimeBack = m_Layer[i]->jscPrime(Side::Back, m_CombinedLayerWavelengths);
            FenestrationCommon::logMsg("before result.addLayer");
            result.addLayer(m_Layer[i]->getResultsAtWavelength(wavelengthIndex),
                            jscPrimeFront[wavelengthIndex],
                            jscPrimeBack[wavelengthIndex]);
        }

        FenestrationCommon::logMsg(
          "end CEquivalentBSDFLayer::getEquivalentLayerAtWavelength with wavelengthIndex = "
          + std::to_string(wavelengthIndex));
        return result;
    }

    std::vector<double> CEquivalentBSDFLayer::unionOfLayerWavelengths(
      const std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & t_Layer)
    {
        FenestrationCommon::logMsg("begin CEquivalentBSDFLayer::unionOfLayerWavelengths");
        FenestrationCommon::CCommonWavelengths wl;
        for(const auto & layer : t_Layer)
        {
            FenestrationCommon::logMsg("in for(const auto & layer : t_Layer)");
            wl.addWavelength(layer->getBandWavelengths());
        }
        FenestrationCommon::logMsg("before result = wl.getCombinedWavelengths");
        auto result = wl.getCombinedWavelengths(FenestrationCommon::Combine::Interpolate);
        FenestrationCommon::logMsg("end CEquivalentBSDFLayer::unionOfLayerWavelengths with result.size = " + std::to_string(result.size()));
        return result;
    }
}   // namespace MultiLayerOptics
