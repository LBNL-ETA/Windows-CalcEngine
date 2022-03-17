#include "EquivalentBSDFLayerSingleBand.hpp"
#include "WCESingleLayerOptics.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;
using namespace SingleLayerOptics;

namespace MultiLayerOptics
{
    SquareMatrix interReflectance(const SquareMatrix & t_Lambda,
                                  const SquareMatrix & t_Rb,
                                  const SquareMatrix & t_Rf)
    {
        const auto size = t_Lambda.size();
        const auto lRb = t_Lambda * t_Rb;
        const auto lRf = t_Lambda * t_Rf;
        auto InterRefl = lRb * lRf;
        SquareMatrix I(size);
        I.setIdentity();
        InterRefl = I - InterRefl;
        InterRefl = InterRefl.inverse();
        return InterRefl;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  CBSDFDoubleLayer
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CBSDFDoubleLayer::CBSDFDoubleLayer(const CBSDFIntegrator & t_FrontLayer,
                                       const CBSDFIntegrator & t_BackLayer)
    {
        const SquareMatrix aLambda = t_FrontLayer.lambdaMatrix();
        const auto InterRefl1 = interReflectance(aLambda,
                                                 t_FrontLayer.at(Side::Back, PropertySimple::R),
                                                 t_BackLayer.at(Side::Front, PropertySimple::R));

        const auto InterRefl2 = interReflectance(aLambda,
                                                 t_BackLayer.at(Side::Front, PropertySimple::R),
                                                 t_FrontLayer.at(Side::Back, PropertySimple::R));

        m_Tf = equivalentT(t_BackLayer.at(Side::Front, PropertySimple::T),
                           InterRefl1,
                           aLambda,
                           t_FrontLayer.at(Side::Front, PropertySimple::T));
        m_Tb = equivalentT(t_FrontLayer.at(Side::Back, PropertySimple::T),
                           InterRefl2,
                           aLambda,
                           t_BackLayer.at(Side::Back, PropertySimple::T));
        m_Rf = equivalentR(t_FrontLayer.at(Side::Front, PropertySimple::R),
                           t_FrontLayer.at(Side::Front, PropertySimple::T),
                           t_FrontLayer.at(Side::Back, PropertySimple::T),
                           t_BackLayer.at(Side::Front, PropertySimple::R),
                           InterRefl2,
                           aLambda);
        m_Rb = equivalentR(t_BackLayer.at(Side::Back, PropertySimple::R),
                           t_BackLayer.at(Side::Back, PropertySimple::T),
                           t_BackLayer.at(Side::Front, PropertySimple::T),
                           t_FrontLayer.at(Side::Back, PropertySimple::R),
                           InterRefl1,
                           aLambda);

        m_Results = std::make_shared<CBSDFIntegrator>(t_FrontLayer);
        m_Results->setResultMatrices(m_Tf, m_Rf, Side::Front);
        m_Results->setResultMatrices(m_Tb, m_Rb, Side::Back);
    }

    std::shared_ptr<CBSDFIntegrator> CBSDFDoubleLayer::value() const
    {
        return m_Results;
    }

    SquareMatrix CBSDFDoubleLayer::equivalentT(const SquareMatrix & t_Tf2,
                                               const SquareMatrix & t_InterRefl,
                                               const SquareMatrix & t_Lambda,
                                               const SquareMatrix & t_Tf1)
    {
        const auto TinterRefl = t_Tf2 * t_InterRefl;
        const auto lambdaTf1 = t_Lambda * t_Tf1;
        return TinterRefl * lambdaTf1;
    }

    SquareMatrix CBSDFDoubleLayer::equivalentR(const SquareMatrix & t_Rf1,
                                               const SquareMatrix & t_Tf1,
                                               const SquareMatrix & t_Tb1,
                                               const SquareMatrix & t_Rf2,
                                               const SquareMatrix & t_InterRefl,
                                               const SquareMatrix & t_Lambda)
    {
        auto TinterRefl = t_Tb1 * t_InterRefl;
        const auto lambdaRf2 = t_Lambda * t_Rf2;
        const auto lambdaTf1 = t_Lambda * t_Tf1;
        TinterRefl = TinterRefl * lambdaRf2;
        TinterRefl = TinterRefl * lambdaTf1;
        return t_Rf1 + TinterRefl;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  CEquivalentBSDFLayerSingleBand
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CEquivalentBSDFLayerSingleBand::CEquivalentBSDFLayerSingleBand(
      const std::shared_ptr<CBSDFIntegrator> & t_Layer) :
        m_PropertiesCalculated(false)
    {
        m_EquivalentLayer = std::make_shared<CBSDFIntegrator>(t_Layer);
        for(Side aSide : EnumSide())
        {
            m_A[aSide] = std::vector<std::vector<double>>();
        }
        m_Layers.push_back(t_Layer);
        m_Lambda = t_Layer->lambdaMatrix();
    }

    SquareMatrix CEquivalentBSDFLayerSingleBand::getMatrix(const Side t_Side,
                                                           const PropertySimple t_Property)
    {
        calcEquivalentProperties();
        return m_EquivalentLayer->getMatrix(t_Side, t_Property);
    }

    SquareMatrix CEquivalentBSDFLayerSingleBand::getProperty(const Side t_Side,
                                                             const PropertySimple t_Property)
    {
        return getMatrix(t_Side, t_Property);
    }

    std::vector<double> CEquivalentBSDFLayerSingleBand::getLayerAbsorptances(const size_t Index,
                                                                             Side t_Side)
    {
        calcEquivalentProperties();
        return m_A.at(t_Side)[Index - 1];
    }

    size_t CEquivalentBSDFLayerSingleBand::getNumberOfLayers() const
    {
        return m_Layers.size();
    }

    void CEquivalentBSDFLayerSingleBand::addLayer(const std::shared_ptr<CBSDFIntegrator> & t_Layer)
    {
        m_Layers.push_back(t_Layer);
        m_PropertiesCalculated = false;
        for(Side aSide : EnumSide())
        {
            m_A.at(aSide).clear();
        }
    }

    void CEquivalentBSDFLayerSingleBand::BuildForwardAndBackwardLayers(size_t numberOfLayers)
    {
        m_EquivalentLayer = m_Layers[0];
        m_Forward.push_back(m_EquivalentLayer);
        for(size_t i = 1; i < numberOfLayers; ++i)
        {
            m_EquivalentLayer = CBSDFDoubleLayer(*m_EquivalentLayer, *m_Layers[i]).value();
            m_Forward.push_back(m_EquivalentLayer);
        }
        m_Backward.push_back(m_EquivalentLayer);

        std::shared_ptr<CBSDFIntegrator> bLayer = m_Layers[numberOfLayers - 1];
        for(size_t i = numberOfLayers - 1; i > 1; --i)
        {
            bLayer = CBSDFDoubleLayer(*m_Layers[i - 1], *bLayer).value();
            m_Backward.push_back(bLayer);
        }
        m_Backward.push_back(m_Layers[numberOfLayers - 1]);
    }

    void CEquivalentBSDFLayerSingleBand::CreateIplusAndIminusValues(size_t numberOfLayers,
                                                                    const size_t matrixSize)
    {
        // Equations used here are from Klems-Matrix Layer calculations- part 2 paper
        for(size_t i = 0; i < numberOfLayers; i++)
        {
            if(i == numberOfLayers - 1)
            {
                SquareMatrix iMinus{matrixSize};
                iMinus.setIdentity();
                m_Iminus[Side::Back].push_back(iMinus);
                SquareMatrix iPlus{matrixSize};
                m_Iplus[Side::Front].push_back(iPlus);
            }
            else
            {
                CBSDFIntegrator & Layer1 = *m_Forward[i];
                CBSDFIntegrator & Layer2 = *m_Backward[i + 1];
                const auto InterRefl2{interReflectance(m_Lambda,
                                                       Layer2.at(Side::Front, PropertySimple::R),
                                                       Layer1.at(Side::Back, PropertySimple::R))};
                const auto Ab{m_Layers[i]->Abs(Side::Back)};
                const auto iMinus{
                  iminusCalc(InterRefl2, Layer2.getMatrix(Side::Back, PropertySimple::T))};
                m_Iminus[Side::Back].push_back(iMinus);
                const auto iPlus{iplusCalc(InterRefl2,
                                           Layer2.getMatrix(Side::Front, PropertySimple::R),
                                           Layer1.getMatrix(Side::Front, PropertySimple::T))};
                m_Iplus[Side::Front].push_back(iPlus);
            }

            if(i == 0)
            {
                SquareMatrix iMinus{matrixSize};
                iMinus.setIdentity();
                m_Iminus[Side::Front].push_back(iMinus);
                SquareMatrix iPlus{matrixSize};
                m_Iplus[Side::Back].push_back(iPlus);
            }
            else
            {
                CBSDFIntegrator & Layer1 = *m_Forward[i - 1];
                CBSDFIntegrator & Layer2 = *m_Backward[i];
                const auto InterRefl1{interReflectance(m_Lambda,
                                                       Layer1.at(Side::Back, PropertySimple::R),
                                                       Layer2.at(Side::Front, PropertySimple::R))};
                const auto Af{m_Layers[i]->Abs(Side::Front)};
                const auto iMinus{
                  iminusCalc(InterRefl1, Layer1.at(Side::Front, PropertySimple::T))};
                m_Iminus[Side::Front].push_back(iMinus);
                const auto iPlus{iplusCalc(InterRefl1,
                                           Layer1.at(Side::Back, PropertySimple::R),
                                           Layer2.at(Side::Back, PropertySimple::T))};
                m_Iplus[Side::Back].push_back(iPlus);
            }
        }
    }

    void CEquivalentBSDFLayerSingleBand::CalculateLayerAbsorptances(size_t numberOfLayers)
    {
        for(size_t i = 0; i < numberOfLayers; i++)
        {
            for(Side aSide : EnumSide())
            {
                auto AbsFront{m_Layers[i]->Abs(aSide)};
                auto AbsBack{m_Layers[i]->Abs(oppositeSide(aSide))};
                auto absorbedFront{AbsFront * m_Iminus.at(aSide)[i]};
                auto absorbedBack{AbsBack * m_Iplus.at(aSide)[i]};
                std::transform(absorbedFront.begin(),
                               absorbedFront.end(),
                               absorbedBack.begin(),
                               absorbedFront.begin(),
                               std::plus<>());
                m_A.at(aSide).push_back(absorbedFront);
            }
        }
    }

    void CEquivalentBSDFLayerSingleBand::calcEquivalentProperties()
    {
        if(!m_PropertiesCalculated)
        {
            // Absorptance calculations need to observe every layer in isolation. For that purpose
            // code bellow will create m_Forward and m_Backward layers
            const auto numberOfLayers{m_Layers.size()};
            BuildForwardAndBackwardLayers(numberOfLayers);

            const auto matrixSize{m_Lambda.size()};
            CreateIplusAndIminusValues(numberOfLayers, matrixSize);

            CalculateLayerAbsorptances(numberOfLayers);
            m_PropertiesCalculated = true;
        }
    }

    SquareMatrix CEquivalentBSDFLayerSingleBand::iminusCalc(const SquareMatrix & t_InterRefl,
                                                            const SquareMatrix & t_T) const
    {
        const auto part2 = m_Lambda * t_T;
        auto part1 = t_InterRefl * part2;
        return part1;
    }

    SquareMatrix CEquivalentBSDFLayerSingleBand::iplusCalc(const SquareMatrix & t_InterRefl,
                                                           const SquareMatrix & t_R,
                                                           const SquareMatrix & t_T) const
    {
        const auto part2 = m_Lambda * t_R;
        const auto part3 = m_Lambda * t_T;
        auto part1 = t_InterRefl * part2;
        part1 = part1 * part3;
        return part1;
    }

}   // namespace MultiLayerOptics
