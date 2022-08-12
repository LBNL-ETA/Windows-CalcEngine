
#include <cmath>
#include <cassert>
#include <stdexcept>
#ifdef STL_MULTITHREADING
#    include <execution>
#endif

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

    void CEquivalentBSDFLayer::setMatrixLayerWavelengths(const std::vector<double> & wavelenghts)
    {
        m_CombinedLayerWavelengths = wavelenghts;
        for(const auto & layer : m_Layer)
        {
            layer->setBandWavelengths(wavelenghts);
        }
    }

    void CEquivalentBSDFLayer::calculate()
    {
        const size_t matrixSize = m_Lambda.size();
        const size_t numberOfLayers = m_Layer.size();

        for(Side aSide : EnumSide())
        {
            m_TotA[aSide] = CMatrixSeries(numberOfLayers, matrixSize);
            m_TotJSC[aSide] = CMatrixSeries(numberOfLayers, matrixSize);
            for(PropertySimple aProperty : EnumPropertySimple())
            {
                m_Tot[{aSide, aProperty}] = CMatrixSeries(matrixSize, matrixSize);
            }
        }

        calculateWavelengthByWavelengthProperties();

        m_Calculated = true;
    }

    void CEquivalentBSDFLayer::calculateWavelengthByWavelengthProperties()
    {
        for(size_t i = 0u; i < m_CombinedLayerWavelengths.size(); ++i)
        {
            auto layer{getEquivalentLayerAtWavelength(i)};
            for(auto aSide : EnumSide())
            {
                const auto numberOfLayers{m_Layer.size()};
                for(size_t layerNumber = 0; layerNumber < numberOfLayers; ++layerNumber)
                {
                    m_TotA.at(aSide).addProperties(
                      layerNumber,
                      m_CombinedLayerWavelengths[i],
                      layer.getLayerAbsorptances(layerNumber + 1, aSide));
                    m_TotJSC.at(aSide).addProperties(layerNumber,
                                                     m_CombinedLayerWavelengths[i],
                                                     layer.getLayerJSC(layerNumber + 1, aSide));
                }
                for(auto aProperty : EnumPropertySimple())
                {
                    m_Tot.at({aSide, aProperty})
                      .addProperties(m_CombinedLayerWavelengths[i],
                                     layer.getProperty(aSide, aProperty));
                }
            }
        }
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
        CCommonWavelengths wl;
        for(const auto & layer : t_Layer)
        {
            wl.addWavelength(layer->getBandWavelengths());
        }

        return wl.getCombinedWavelengths(Combine::Interpolate);
    }
}   // namespace MultiLayerOptics
