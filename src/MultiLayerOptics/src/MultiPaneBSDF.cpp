#include <cmath>
#include <numeric>
#include <cassert>
#include <utility>

#include "MultiPaneBSDF.hpp"
#include "EquivalentBSDFLayerSingleBand.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"

using FenestrationCommon::IntegrationType;
using FenestrationCommon::Side;
using FenestrationCommon::Property;
using FenestrationCommon::PropertySimple;
using FenestrationCommon::Scattering;
using FenestrationCommon::ScatteringSimple;
using FenestrationCommon::SquareMatrix;
using FenestrationCommon::CSeries;
using FenestrationCommon::CMatrixSeries;

using SingleLayerOptics::CBSDFLayer;
using SingleLayerOptics::BSDFDirection;

namespace SingleLayerOptics
{
    class PhotovoltaicSpecularBSDFLayer;
}

namespace MultiLayerOptics
{
    CMultiPaneBSDF::CMultiPaneBSDF(const std::vector<std::shared_ptr<CBSDFLayer>> & t_Layer,
                                   const std::optional<std::vector<double>> & matrixWavelengths,
                                   const FenestrationCommon::ProgressCallback & callback) :
        m_EquivalentLayer(t_Layer, matrixWavelengths),
        m_Results(t_Layer[0]->getDirections(BSDFDirection::Incoming)),
        m_Calculated(false),
        m_MinLambdaCalculated(0),
        m_MaxLambdaCalculated(0),
        m_BSDFDirections(t_Layer[0]->getDirections(BSDFDirection::Incoming))
    {
        m_EquivalentLayer.calculate(callback);
    }

    std::vector<std::vector<double>>
      CMultiPaneBSDF::calcPVLayersElectricity(const std::vector<std::vector<double>> & jsc,
                                              const std::vector<double> & incomingSolar)
    {
        std::vector<std::vector<double>> result;
        const auto & layers{m_EquivalentLayer.getLayers()};
        assert(layers.size() == jsc.size());
        for(size_t i = 0u; i < layers.size(); ++i)
        {
            std::vector<double> power;
            const auto voc{layers[i]->voc(jsc[i])};
            const auto ff{layers[i]->ff(jsc[i])};
            for(size_t j = 0u; j < jsc[i].size(); ++j)
            {
                power.emplace_back(voc[j] * ff[j] * jsc[i][j] / incomingSolar[j]);
            }
            result.emplace_back(power);
        }

        return result;
    }

    SquareMatrix CMultiPaneBSDF::getMatrix(const double minLambda,
                                           const double maxLambda,
                                           const Side t_Side,
                                           const PropertySimple t_Property)
    {
        calculate(minLambda, maxLambda);

        return m_Results.getMatrix(t_Side, t_Property);
    }

    double CMultiPaneBSDF::DirDir(const double minLambda,
                                  const double maxLambda,
                                  const Side t_Side,
                                  const PropertySimple t_Property,
                                  const double t_Theta,
                                  const double t_Phi)
    {
        calculate(minLambda, maxLambda);

        return m_Results.DirDir(t_Side, t_Property, t_Theta, t_Phi);
    }

    double CMultiPaneBSDF::DirDir(const double minLambda,
                                  const double maxLambda,
                                  const Side t_Side,
                                  const PropertySimple t_Property,
                                  const size_t Index)
    {
        calculate(minLambda, maxLambda);

        return m_Results.DirDir(t_Side, t_Property, Index);
    }

    SquareMatrix CMultiPaneBSDF::calculateProperties(Side aSide,
                                                     PropertySimple aProperty,
                                                     const double minLambda,
                                                     const double maxLambda)
    {
        CMatrixSeries aTot = m_EquivalentLayer.getTotal(aSide, aProperty);
        if(m_SpectralIntegrationWavelengths.has_value())
        {
            aTot.interpolate(m_SpectralIntegrationWavelengths.value());
        }
        m_WavelengthMatrices[std::make_pair(aSide, aProperty)] = aTot.seriesMatrices();
        aTot.mMult(m_IncomingSpectra);
        aTot.integrate(m_CalculationProperties.m_IntegrationType,
                       m_CalculationProperties.m_NormalizationCoefficient,
                       m_SpectralIntegrationWavelengths);

        return aTot.getSquaredMatrixSums(minLambda, maxLambda, m_IncomingSolar);
    }

    std::vector<std::vector<double>>
      CMultiPaneBSDF::calculateJSC(Side aSide, const double minLambda, const double maxLambda)
    {
        CMatrixSeries jscTotal = m_EquivalentLayer.getTotalJSC(aSide);
        jscTotal.integrate(m_CalculationProperties.m_IntegrationType,
                           m_CalculationProperties.m_NormalizationCoefficient,
                           m_SpectralIntegrationWavelengths);
        auto jscSum = jscTotal.getSums(minLambda, maxLambda);

        std::vector<std::vector<double>> jscWithSolar;
        for(size_t i = 0u; i < jscSum.size(); ++i)
        {
            jscWithSolar.emplace_back();
            for(size_t j = 0u; j < jscSum[i].size(); ++j)
            {
                jscWithSolar[i].push_back(jscSum[i][j] * m_IncomingSolar[i]);
            }
        }

        return calcPVLayersElectricity(jscWithSolar, m_IncomingSolar);
    }

    std::vector<std::vector<double>> CMultiPaneBSDF::calculateAbsorptance(Side aSide,
                                                                          const double minLambda,
                                                                          const double maxLambda)
    {
        CMatrixSeries aTotalA = m_EquivalentLayer.getTotalA(aSide);
        if(m_SpectralIntegrationWavelengths.has_value())
        {
            aTotalA.interpolate(m_SpectralIntegrationWavelengths.value());
        }
        aTotalA.mMult(m_IncomingSpectra);
        aTotalA.integrate(m_CalculationProperties.m_IntegrationType,
                          m_CalculationProperties.m_NormalizationCoefficient,
                          m_SpectralIntegrationWavelengths);
        return aTotalA.getSums(minLambda, maxLambda, m_IncomingSolar);
    }

    std::vector<double> CMultiPaneBSDF::calculateIncomingSolar(
      const std::vector<FenestrationCommon::CSeries> & incomingSpectra,
      const double minLambda,
      const double maxLambda)
    {
        std::vector<double> incomingSolar;
        incomingSolar.reserve(incomingSpectra.size());
        std::transform(begin(incomingSpectra),
                       end(incomingSpectra),
                       std::back_inserter(incomingSolar),
                       [this, minLambda, maxLambda](const CSeries & aSpectra) {
                           return aSpectra
                             .integrate(m_CalculationProperties.m_IntegrationType,
                                        m_CalculationProperties.m_NormalizationCoefficient,
                                        m_SpectralIntegrationWavelengths)
                             .sum(minLambda, maxLambda);
                       });

        return incomingSolar;
    }

    void CMultiPaneBSDF::calculate(const double minLambda, const double maxLambda)
    {
        if(m_Calculated && minLambda == m_MinLambdaCalculated && maxLambda == m_MaxLambdaCalculated)
        {
            return;
        }

        m_IncomingSolar = calculateIncomingSolar(m_IncomingSpectra, minLambda, maxLambda);

        FenestrationCommon::EnumSide sides;
        for(Side aSide : sides)
        {
            m_Abs[aSide] = calculateAbsorptance(aSide, minLambda, maxLambda);
            m_AbsElectricity[aSide] = calculateJSC(aSide, minLambda, maxLambda);

            std::map<std::pair<Side, PropertySimple>, SquareMatrix> aResults;
            FenestrationCommon::EnumPropertySimple properties;
            for(PropertySimple aProperty : properties)
            {
                aResults[{aSide, aProperty}] =
                  calculateProperties(aSide, aProperty, minLambda, maxLambda);
            }

            m_Results.setMatrices(aResults.at({aSide, PropertySimple::T}),
                                  aResults.at({aSide, PropertySimple::R}),
                                  aSide);
        }

        m_Results.resetCalculatedResults();

        for(Side aSide : sides)
        {
            calcHemisphericalAbs(aSide);
        }

        m_MinLambdaCalculated = minLambda;
        m_MaxLambdaCalculated = maxLambda;
        m_Calculated = true;
    }

    double CMultiPaneBSDF::integrateBSDFAbsorptance(const std::vector<double> & lambda,
                                                    const std::vector<double> & absorptance)
    {
        assert(absorptance.size() == lambda.size());
        using ConstantsData::WCE_PI;
        const auto mult{FenestrationCommon::mult<double>(lambda, absorptance)};
        return std::accumulate(mult.begin(), mult.end(), 0.0) / WCE_PI;
    }

    void CMultiPaneBSDF::calcHemisphericalAbs(const Side t_Side)
    {
        m_AbsHem[t_Side].clear();
        m_AbsHemElectricity[t_Side].clear();
        const size_t numOfLayers = m_Abs[t_Side].size();
        for(size_t layNum = 0; layNum < numOfLayers; ++layNum)
        {
            m_AbsHem[t_Side].push_back(
              integrateBSDFAbsorptance(m_Results.lambdaVector(), m_Abs[t_Side][layNum]));
            m_AbsHemElectricity[t_Side].push_back(
              integrateBSDFAbsorptance(m_Results.lambdaVector(), m_AbsElectricity[t_Side][layNum]));
        }
    }

    std::vector<double> CMultiPaneBSDF::getCommonWavelengthsFromLayers(
      const std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & t_Layer) const
    {
        FenestrationCommon::CCommonWavelengths cw;
        for(const auto & layer : t_Layer)
        {
            cw.addWavelength(layer->getBandWavelengths());
        }
        return cw.getCombinedWavelengths(FenestrationCommon::Combine::Interpolate);
    }

    std::vector<double>
      CMultiPaneBSDF::Abs(double minLambda, double maxLambda, Side t_Side, size_t Index)
    {
        calculate(minLambda, maxLambda);
        return m_Abs.at(t_Side)[Index - 1];
    }

    std::vector<double>
      CMultiPaneBSDF::AbsHeat(double minLambda, double maxLambda, Side t_Side, size_t Index)
    {
        calculate(minLambda, maxLambda);
        std::vector<double> result;
        for(size_t i = 0u; i < m_Abs.at(t_Side)[Index - 1].size(); ++i)
        {
            result.push_back(m_Abs.at(t_Side)[Index - 1][i]
                             - m_AbsElectricity.at(t_Side)[Index - 1][i]);
        }

        return result;
    }

    std::vector<double>
      CMultiPaneBSDF::AbsElectricity(double minLambda, double maxLambda, Side t_Side, size_t Index)
    {
        calculate(minLambda, maxLambda);
        return m_AbsElectricity.at(t_Side)[Index - 1];
    }

    std::vector<double> CMultiPaneBSDF::DirHem(const double minLambda,
                                               const double maxLambda,
                                               const Side t_Side,
                                               const PropertySimple t_Property)
    {
        calculate(minLambda, maxLambda);
        return m_Results.DirHem(t_Side, t_Property);
    }

    double CMultiPaneBSDF::DirHem(const double minLambda,
                                  const double maxLambda,
                                  const Side t_Side,
                                  const PropertySimple t_Property,
                                  const double t_Theta,
                                  const double t_Phi)
    {
        const auto aIndex = m_Results.getNearestBeamIndex(t_Theta, t_Phi);
        return DirHem(minLambda, maxLambda, t_Side, t_Property)[aIndex];
    }

    double CMultiPaneBSDF::DirHem(const double minLambda,
                                  const double maxLambda,
                                  const Side t_Side,
                                  const PropertySimple t_Property,
                                  const size_t Index)
    {
        return DirHem(minLambda, maxLambda, t_Side, t_Property)[Index];
    }

    double CMultiPaneBSDF::Abs(const double minLambda,
                               const double maxLambda,
                               const Side t_Side,
                               const size_t layerIndex,
                               const double t_Theta,
                               const double t_Phi)
    {
        auto aIndex = m_Results.getNearestBeamIndex(t_Theta, t_Phi);
        return Abs(minLambda, maxLambda, t_Side, layerIndex)[aIndex];
    }

    double CMultiPaneBSDF::AbsHeat(double minLambda,
                                   double maxLambda,
                                   Side t_Side,
                                   size_t layerIndex,
                                   double t_Theta,
                                   double t_Phi)
    {
        return Abs(minLambda, maxLambda, t_Side, layerIndex, t_Theta, t_Phi)
               - AbsElectricity(minLambda, maxLambda, t_Side, layerIndex, t_Theta, t_Phi);
    }

    double CMultiPaneBSDF::AbsElectricity(double minLambda,
                                          double maxLambda,
                                          Side t_Side,
                                          size_t layerIndex,
                                          double t_Theta,
                                          double t_Phi)
    {
        auto aIndex = m_Results.getNearestBeamIndex(t_Theta, t_Phi);
        return AbsElectricity(minLambda, maxLambda, t_Side, layerIndex)[aIndex];
    }

    double CMultiPaneBSDF::Abs(const double minLambda,
                               const double maxLambda,
                               const Side t_Side,
                               const size_t layerIndex,
                               const size_t beamIndex)
    {
        return Abs(minLambda, maxLambda, t_Side, layerIndex)[beamIndex];
    }

    double CMultiPaneBSDF::DiffDiff(const double minLambda,
                                    const double maxLambda,
                                    const Side t_Side,
                                    const PropertySimple t_Property)
    {
        calculate(minLambda, maxLambda);
        return m_Results.DiffDiff(t_Side, t_Property);
    }

    double CMultiPaneBSDF::AbsDiff(const double minLambda,
                                   const double maxLambda,
                                   const Side t_Side,
                                   const size_t t_LayerIndex)
    {
        calculate(minLambda, maxLambda);
        return m_AbsHem[t_Side][t_LayerIndex - 1];
    }

    double CMultiPaneBSDF::AbsDiffHeat(double minLambda,
                                       double maxLambda,
                                       FenestrationCommon::Side t_Side,
                                       size_t t_LayerIndex)
    {
        return AbsDiff(minLambda, maxLambda, t_Side, t_LayerIndex)
               - AbsDiffElectricity(minLambda, maxLambda, t_Side, t_LayerIndex);
    }

    double CMultiPaneBSDF::AbsDiffElectricity(double minLambda,
                                              double maxLambda,
                                              FenestrationCommon::Side t_Side,
                                              size_t t_LayerIndex)
    {
        calculate(minLambda, maxLambda);
        return m_AbsHemElectricity[t_Side][t_LayerIndex - 1];
    }

    double CMultiPaneBSDF::energy(const double minLambda,
                                  const double maxLambda,
                                  const Side t_Side,
                                  const PropertySimple t_Property,
                                  const double t_Theta,
                                  const double t_Phi)
    {
        calculate(minLambda, maxLambda);
        const auto aIndex = m_Results.getNearestBeamIndex(t_Theta, t_Phi);
        const auto solarRadiation = m_IncomingSolar[aIndex];
        const auto dirHem = DirHem(minLambda, maxLambda, t_Side, t_Property)[aIndex];
        return dirHem * solarRadiation;
    }

    double CMultiPaneBSDF::energyAbs(const double minLambda,
                                     const double maxLambda,
                                     const Side t_Side,
                                     const size_t Index,
                                     const double t_Theta,
                                     const double t_Phi)
    {
        calculate(minLambda, maxLambda);
        auto aIndex = m_Results.getNearestBeamIndex(t_Theta, t_Phi);
        double solarRadiation = m_IncomingSolar[aIndex];
        double abs = Abs(minLambda, maxLambda, t_Side, Index)[aIndex];
        return abs * solarRadiation;
    }

    std::unique_ptr<CMultiPaneBSDF> CMultiPaneBSDF::create(
      const std::vector<std::shared_ptr<SingleLayerOptics::CBSDFLayer>> & t_Layer,
      const std::optional<std::vector<double>> & matrixWavelengths,
      const FenestrationCommon::ProgressCallback & callback)
    {
        return std::unique_ptr<CMultiPaneBSDF>(
          new CMultiPaneBSDF(t_Layer, matrixWavelengths, callback));
    }

    double CMultiPaneBSDF::getPropertySimple(const double minLambda,
                                             const double maxLambda,
                                             const FenestrationCommon::PropertySimple t_Property,
                                             const FenestrationCommon::Side t_Side,
                                             const FenestrationCommon::Scattering t_Scattering,
                                             const double t_Theta,
                                             const double t_Phi)
    {
        double result{0};
        switch(t_Scattering)
        {
            case Scattering::DirectDirect:
                result = DirDir(minLambda, maxLambda, t_Side, t_Property, t_Theta, t_Phi);
                break;
            case Scattering::DirectDiffuse:
                result = DirHem(minLambda, maxLambda, t_Side, t_Property, t_Theta, t_Phi)
                         - DirDir(minLambda, maxLambda, t_Side, t_Property, t_Theta, t_Phi);
                break;
            case Scattering::DirectHemispherical:
                result = DirHem(minLambda, maxLambda, t_Side, t_Property, t_Theta, t_Phi);
                break;
            case Scattering::DiffuseDiffuse:
                result = DiffDiff(minLambda, maxLambda, t_Side, t_Property);
                break;
        }
        return result;
    }
    std::vector<double> CMultiPaneBSDF::getWavelengths() const
    {
        return m_EquivalentLayer.getCommonWavelengths();
    }

    std::vector<FenestrationCommon::MatrixAtWavelength> CMultiPaneBSDF::getWavelengthMatrices(
      double minLambda, double maxLambda, Side t_Side, PropertySimple t_Property)
    {
        calculate(minLambda, maxLambda);
        return m_WavelengthMatrices.at(std::make_pair(t_Side, t_Property));
    }

    double CMultiPaneBSDF::getMinLambda() const
    {
        return m_EquivalentLayer.getMinLambda();
    }
    double CMultiPaneBSDF::getMaxLambda() const
    {
        return m_EquivalentLayer.getMaxLambda();
    }

    void CMultiPaneBSDF::setCalculationProperties(
      const SingleLayerOptics::CalculationProperties & calcProperties)
    {
        m_CalculationProperties = calcProperties;

        const auto directionsSize{m_BSDFDirections.size()};

        this->m_IncomingSpectra = std::vector<CSeries>(directionsSize);
        for(size_t i = 0; i < directionsSize; ++i)
        {
            this->m_IncomingSpectra[i] = calcProperties.scaledSolarRadiation();
        }
        m_SpectralIntegrationWavelengths = calcProperties.CommonWavelengths;

        m_Calculated = false;
    }

    std::vector<double>
      CMultiPaneBSDF::getAbsorptanceLayers(const double minLambda,
                                           const double maxLambda,
                                           const FenestrationCommon::Side side,
                                           const FenestrationCommon::ScatteringSimple scattering,
                                           const double theta,
                                           const double phi)
    {
        std::vector<double> abs;
        const size_t absSize{m_EquivalentLayer.numberOfLayers()};
        for(size_t i = 1u; i <= absSize; ++i)
        {
            switch(scattering)
            {
                case ScatteringSimple::Direct:
                    abs.push_back(Abs(minLambda, maxLambda, side, i, theta, phi));
                    break;
                case ScatteringSimple::Diffuse:
                    abs.push_back(AbsDiff(minLambda, maxLambda, side, i));
                    break;
            }
        }
        return abs;
    }

    std::vector<double>
      CMultiPaneBSDF::getAbsorptanceLayersHeat(const double minLambda,
                                               const double maxLambda,
                                               FenestrationCommon::Side side,
                                               FenestrationCommon::ScatteringSimple scattering,
                                               const double theta,
                                               const double phi)
    {
        std::vector<double> abs;
        const size_t absSize{m_EquivalentLayer.numberOfLayers()};
        for(size_t i = 1u; i <= absSize; ++i)
        {
            switch(scattering)
            {
                case ScatteringSimple::Direct:
                    abs.push_back(AbsHeat(minLambda, maxLambda, side, i, theta, phi));
                    break;
                case ScatteringSimple::Diffuse:
                    abs.push_back(AbsDiffHeat(minLambda, maxLambda, side, i));
                    break;
            }
        }
        return abs;
    }
    std::vector<double> CMultiPaneBSDF::getAbsorptanceLayersElectricity(
      const double minLambda,
      const double maxLambda,
      FenestrationCommon::Side side,
      FenestrationCommon::ScatteringSimple scattering,
      const double theta,
      const double phi)
    {
        std::vector<double> abs;
        size_t absSize{m_EquivalentLayer.numberOfLayers()};
        for(size_t i = 1u; i <= absSize; ++i)
        {
            switch(scattering)
            {
                case ScatteringSimple::Direct:
                    abs.push_back(AbsElectricity(minLambda, maxLambda, side, i, theta, phi));
                    break;
                case ScatteringSimple::Diffuse:
                    abs.push_back(AbsDiffElectricity(minLambda, maxLambda, side, i));
                    break;
            }
        }
        return abs;
    }

    double CMultiPaneBSDF::DirDiff(double minLambda,
                                   double maxLambda,
                                   FenestrationCommon::Side t_Side,
                                   FenestrationCommon::PropertySimple t_Property,
                                   double t_Theta,
                                   double t_Phi)
    {
        return DirHem(minLambda, maxLambda, t_Side, t_Property, t_Theta, t_Phi)
               - DirDir(minLambda, maxLambda, t_Side, t_Property, t_Theta, t_Phi);
    }

}   // namespace MultiLayerOptics
