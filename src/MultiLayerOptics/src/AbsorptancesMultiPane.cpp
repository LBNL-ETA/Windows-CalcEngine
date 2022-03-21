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
        m_Rf.push_back(t_Rf);
        m_Rb.push_back(t_Rb);
    }

    void CAbsorptancesMultiPane::addLayer(const CSeries & t_T,
                                          const CSeries & t_Rf,
                                          const CSeries & t_Rb)
    {
        m_T.push_back(t_T);
        m_Rf.push_back(t_Rf);
        m_Rb.push_back(t_Rb);
        m_StateCalculated = false;
    }

    CSeries CAbsorptancesMultiPane::Abs(const size_t Index)
    {
        calculateState();
        return m_Abs[Index];
    }

    size_t CAbsorptancesMultiPane::numOfLayers()
    {
        calculateState();
        return m_Abs.size();
    }

    CSeries CAbsorptancesMultiPane::iplus(size_t Index)
    {
        calculateState();
        return Iplus[Index];
    }

    CSeries CAbsorptancesMultiPane::iminus(size_t Index)
    {
        calculateState();
        return Iminus[Index];
    }

    void CAbsorptancesMultiPane::calculateRTCoefficients()
    {
        const size_t size{m_T.size()};

        // Calculate r and t coefficients
        CSeries r;
        CSeries t;
        const auto wv {m_T[size - 1].getXArray()};
        r.setConstantValues(wv, 0);
        t.setConstantValues(wv, 0);
        m_rCoeffs.clear();
        m_tCoeffs.clear();

        // layers loop
        for(int i = static_cast<int>(size) - 1; i >= 0; --i)
        {
            t = tCoeffs(m_T[i], m_Rb[i], r);
            r = rCoeffs(m_T[i], m_Rf[i], m_Rb[i], r);

            m_rCoeffs.insert(m_rCoeffs.begin(), r);
            m_tCoeffs.insert(m_tCoeffs.begin(), t);
        }
    }

    void CAbsorptancesMultiPane::calculateNormalizedRadiances()
    {
        // Calculate normalized radiances
        const auto wv{m_T[0].getXArray()};
        const size_t size{m_rCoeffs.size()};

        CSeries Im;
        CSeries Ip;
        Im.setConstantValues(wv, 1);
        Iminus.push_back(Im);

        for(size_t i = 0; i < size; ++i)
        {
            Ip = m_rCoeffs[i] * Im;
            Im = m_tCoeffs[i] * Im;
            if(i != 0u)
            {
                Iplus.push_back(Ip);
            }
            Iminus.push_back(Im);
        }
        Ip.setConstantValues(wv, 0);
        Iplus.push_back(Ip);
    }

    void CAbsorptancesMultiPane::calculateAbsorptances()
    {
        // Calculate absorptances
        const size_t size{Iminus.size()};
        m_Abs.clear();
        for(size_t i = 0; i < size - 1; ++i)
        {
            const auto Afront{1 - m_T[i] - m_Rf[i]};
            const auto Aback{1 - m_T[i] - m_Rb[i]};
            const auto Ifront = Iminus[i] * Afront;
            const auto Iback = Iplus[i] * Aback;
            m_Abs.emplace_back(Ifront + Iback);
        }
    }

    void CAbsorptancesMultiPane::calculateState()
    {
        if(!m_StateCalculated)
        {
            Iplus.clear();
            Iminus.clear();

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
        size_t size = t_T.size();

        for(size_t i = 0; i < size; ++i)
        {
            double wl = t_T[i].x();
            double rValue = t_Rf[i].value()
                            + t_T[i].value() * t_T[i].value() * t_RCoeffs[i].value()
                                / (1 - t_Rb[i].value() * t_RCoeffs[i].value());
            rCoeffs.addProperty(wl, rValue);
        }

        return rCoeffs;
    }

    CSeries CAbsorptancesMultiPane::tCoeffs(const CSeries & t_T,
                                            const CSeries & t_Rb,
                                            const CSeries & t_RCoeffs)
    {
        CSeries tCoeffs;
        size_t size = t_T.size();

        for(size_t i = 0; i < size; ++i)
        {
            double wl = t_T[i].x();
            double tValue = t_T[i].value() / (1 - t_Rb[i].value() * t_RCoeffs[i].value());
            tCoeffs.addProperty(wl, tValue);
        }

        return tCoeffs;
    }

}   // namespace MultiLayerOptics
