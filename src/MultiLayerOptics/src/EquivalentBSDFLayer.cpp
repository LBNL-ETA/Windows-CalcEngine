
#include <cmath>
#include <cassert>
#include <stdexcept>

#include "EquivalentBSDFLayer.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;
using namespace SingleLayerOptics;

namespace MultiLayerOptics
{
    CEquivalentBSDFLayer::CEquivalentBSDFLayer(const std::vector<double> & t_CommonWavelengths) :
        m_CombinedLayerWavelengths(t_CommonWavelengths),
        m_Calculated(false)
    {}

    void CEquivalentBSDFLayer::addLayer(const std::shared_ptr<CBSDFLayer> & t_Layer)
    {
        t_Layer->setBandWavelengths(m_CombinedLayerWavelengths);
        if(m_Layer.empty())
        {
            m_Lambda = t_Layer->getResults().lambdaMatrix();
        }
        updateWavelengthLayers(*t_Layer);
        m_Layer.push_back(t_Layer);
    }

    const BSDFDirections & CEquivalentBSDFLayer::getDirections(const BSDFDirection t_Side) const
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
        return m_Tot.at(std::make_pair(t_Side, t_Property));
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

    std::vector<std::shared_ptr<CBSDFLayer>> & CEquivalentBSDFLayer::getLayers()
    {
        return m_Layer;
    }

    size_t CEquivalentBSDFLayer::numberOfLayers() const
    {
        return m_Layer.size();
    }

    void CEquivalentBSDFLayer::calculate()
    {
        size_t matrixSize = m_Lambda.size();
        size_t numberOfLayers = m_LayersWL[0].getNumberOfLayers();

        for(Side aSide : EnumSide())
        {
            m_TotA[aSide] = CMatrixSeries(numberOfLayers, matrixSize);
            m_TotJSC[aSide] = CMatrixSeries(numberOfLayers, matrixSize);
            for(PropertySimple aProperty : EnumPropertySimple())
            {
                m_Tot[{aSide, aProperty}] = CMatrixSeries(matrixSize, matrixSize);
            }
        }

        // Calculate total transmitted solar per matrix and perform integration over each wavelength
        const auto WLsize{m_CombinedLayerWavelengths.size()};

        calculateWavelengthProperties(numberOfLayers, 0, WLsize);

        m_Calculated = true;
    }

    void CEquivalentBSDFLayer::calculateWavelengthProperties(size_t const t_NumOfLayers,
                                                             size_t const t_Start,
                                                             size_t const t_End)
    {
        for(auto i = t_Start; i < t_End; ++i)
        {
            const auto curWL = m_CombinedLayerWavelengths[i];

            for(auto aSide : EnumSide())
            {
                for(size_t k = 0; k < t_NumOfLayers; ++k)
                {
                    m_TotA.at(aSide).addProperties(
                      k, curWL, m_LayersWL[i].getLayerAbsorptances(k + 1, aSide));
                    m_TotJSC.at(aSide).addProperties(
                      k, curWL, m_LayersWL[i].getLayerJSC(k + 1, aSide));
                }
                for(auto aProperty : EnumPropertySimple())
                {
                    auto curPropertyMatrix = m_LayersWL[i].getProperty(aSide, aProperty);
                    m_Tot.at(std::make_pair(aSide, aProperty))
                      .addProperties(curWL, curPropertyMatrix);
                }
            }
        }
    }

    void CEquivalentBSDFLayer::updateWavelengthLayers(CBSDFLayer & t_Layer)
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

            const BSDFIntegrator currentLayer = aResults[static_cast<size_t>(index)];

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

}   // namespace MultiLayerOptics
