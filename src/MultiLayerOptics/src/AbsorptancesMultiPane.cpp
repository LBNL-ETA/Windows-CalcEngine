#include <algorithm>

#include "AbsorptancesMultiPane.hpp"
#include "WCECommon.hpp"

using namespace FenestrationCommon;

namespace MultiLayerOptics
{
    CAbsorptancesMultiPane::CAbsorptancesMultiPane(const CSeries & t_T,
                                                   const CSeries & t_Rf,
                                                   const CSeries & t_Rb) :
        m_StateCalculated(false)
    {
        m_T.push_back(t_T);
        m_R.at(Side::Front).push_back(t_Rf);
        m_R.at(Side::Back).push_back(t_Rb);
    }

    void CAbsorptancesMultiPane::addLayer(const CSeries & t_T,
                                          const CSeries & t_Rf,
                                          const CSeries & t_Rb)
    {
        m_T.push_back(t_T);
        m_R.at(Side::Front).push_back(t_Rf);
        m_R.at(Side::Back).push_back(t_Rb);
        m_StateCalculated = false;
    }

    CSeries CAbsorptancesMultiPane::Abs(const size_t Index, Side side)
    {
        calculateState();
        return m_Abs.at(side)[Index];
    }

    size_t CAbsorptancesMultiPane::numOfLayers()
    {
        calculateState();
        return m_Abs.size();
    }

    CSeries CAbsorptancesMultiPane::iplus(size_t Index, Side side)
    {
        calculateState();
        return Iplus.at(side)[Index];
    }

    CSeries CAbsorptancesMultiPane::iminus(size_t Index, Side side)
    {
        calculateState();
        return Iminus.at(side)[Index];
    }

    void CAbsorptancesMultiPane::calculateRTCoefficients()
    {
        const size_t size{m_T.size()};
        for(const auto side : allSides())
        {
            const auto oppositeSide{FenestrationCommon::oppositeSide(side)};

            CSeries r;
            CSeries t;
            const auto wv{m_T[size - 1].getXArray()};
            r.setConstantValues(wv, 0);
            t.setConstantValues(wv, 0);
            m_rCoeffs.at(side).clear();
            m_tCoeffs.at(side).clear();
            m_rCoeffs.at(side).reserve(size);
            m_tCoeffs.at(side).reserve(size);

            auto index{side == Side::Front ? static_cast<int>(size) - 1 : 0};
            const auto endValue{side == Side::Front ? -1 : static_cast<int>(size)};
            do
            {
                t = tCoeffs(m_T[index], m_R.at(oppositeSide)[index], r);
                r = rCoeffs(m_T[index], m_R.at(side)[index], m_R.at(oppositeSide)[index], r);

                m_rCoeffs.at(side).push_back(r);
                m_tCoeffs.at(side).push_back(t);
                if(side == Side::Front)
                {
                    index--;
                }
                else
                {
                    index++;
                }
            } while(index != endValue);

            std::reverse(m_rCoeffs.at(side).begin(), m_rCoeffs.at(side).end());
            std::reverse(m_tCoeffs.at(side).begin(), m_tCoeffs.at(side).end());
        }
    }

    void CAbsorptancesMultiPane::calculateNormalizedRadiances()
    {
        // Calculate normalized radiances
        const auto wv{m_T[0].getXArray()};

        for(const auto side : allSides())
        {
            const size_t size{m_rCoeffs.at(side).size()};

            Iplus.at(side).clear();
            Iminus.at(side).clear();

            CSeries Im;
            CSeries Ip;
            Im.setConstantValues(wv, 1);
            Iminus.at(side).push_back(Im);

            for(size_t i = 0; i < size; ++i)
            {
                Ip = m_rCoeffs.at(side)[i] * Im;
                Im = m_tCoeffs.at(side)[i] * Im;
                if(i != 0u)
                {
                    Iplus.at(side).push_back(Ip);
                }
                Iminus.at(side).push_back(Im);
            }
            Ip.setConstantValues(wv, 0);
            Iplus.at(side).push_back(Ip);
        }
    }

    void CAbsorptancesMultiPane::calculateAbsorptances()
    {
        for(const auto side : allSides())
        {
            const auto oppositeSide{FenestrationCommon::oppositeSide(side)};
            const size_t size{Iminus.at(side).size()};
            m_Abs.at(side).clear();
            for(size_t i = 0; i < size - 1; ++i)
            {
                const auto index{side == Side::Front ? i : size - i - 2u};
                const auto Afront{1 - m_T[index] - m_R.at(side)[index]};
                const auto Aback{1 - m_T[index] - m_R.at(oppositeSide)[index]};
                const auto Ifront = Iminus.at(side)[i] * Afront;
                const auto Iback = Iplus.at(side)[i] * Aback;
                m_Abs.at(side).emplace_back(Ifront + Iback);
            }
        }
    }

    void CAbsorptancesMultiPane::calculateState()
    {
        if(!m_StateCalculated)
        {
            calculateRTCoefficients();

            calculateNormalizedRadiances();

            calculateAbsorptances();
        }
    }

    CSeries CAbsorptancesMultiPane::rCoeffs(const CSeries & t_T,
                                            const CSeries & t_Rf,
                                            const CSeries & t_Rb,
                                            const CSeries & t_RCoeffs)
    {
        CSeries rCoeffs;
        const size_t size = t_T.size();
        rCoeffs.reserve(size);

        for(size_t i = 0; i < size; ++i)
        {
            const double wl = t_T[i].x();
            const double tVal = t_T[i].value();
            const double rbVal = t_Rb[i].value();
            const double rCoeffsVal = t_RCoeffs[i].value();
            const double rValue = t_Rf[i].value()
                            + tVal * tVal * rCoeffsVal / (1 - rbVal * rCoeffsVal);
            rCoeffs.addProperty(wl, rValue);
        }

        return rCoeffs;
    }

    CSeries CAbsorptancesMultiPane::tCoeffs(const CSeries & t_T,
                                            const CSeries & t_Rb,
                                            const CSeries & t_RCoeffs)
    {
        CSeries tCoeffs;
        const size_t size = t_T.size();
        tCoeffs.reserve(size);

        for(size_t i = 0; i < size; ++i)
        {
            const double wl = t_T[i].x();
            const double rbVal = t_Rb[i].value();
            const double rCoeffsVal = t_RCoeffs[i].value();
            const double tValue = t_T[i].value() / (1 - rbVal * rCoeffsVal);
            tCoeffs.addProperty(wl, tValue);
        }

        return tCoeffs;
    }

}   // namespace MultiLayerOptics
