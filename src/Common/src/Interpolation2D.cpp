#include "Interpolation2D.hpp"
#include "WCECommon.hpp"

namespace FenestrationCommon
{
    //////////////////////////////////////////////////////////////////////////////////////
    // SPChipInterpolation2D
    //////////////////////////////////////////////////////////////////////////////////////

    SPChipInterpolation2D::SPChipInterpolation2D(
      const std::vector<TableValue> & t_Points) : m_Points(t_Points)
    {
        m_Hs = calculateHs();
        m_Deltas = calculateDeltas();
        m_Derivatives = calculateDerivatives();
    }

    double SPChipInterpolation2D::value(double const t_Value) const
    {
        if(t_Value <= m_Points.begin()->x)
        {
            return m_Points.begin()->y;
        }

        if(t_Value >= (m_Points.end() - 1)->x)
        {
            return (m_Points.end() - 1)->y;
        }

        auto subinterval = getSubinterval(t_Value);
        auto s = t_Value - m_Points[subinterval].x;
        auto h = m_Hs[subinterval];

        auto y_k = m_Points[subinterval].y;
        auto y_k_plus_1 = m_Points[subinterval + 1].y;
        auto d_k = m_Derivatives[subinterval];
        auto d_k_plus_1 = m_Derivatives[subinterval + 1];
        return interpolate(h, s, y_k, y_k_plus_1, d_k, d_k_plus_1);
    }

    size_t SPChipInterpolation2D::getSubinterval(double const t_Value) const
    {
        size_t interval = 1;
        for(auto i = 1u; i < m_Points.size(); ++i)
        {
            if(m_Points[i].x > t_Value)
            {
                interval = i - 1;
                break;
            }
        }
        return interval;
    }

    std::vector<double> SPChipInterpolation2D::calculateHs() const
    {
        std::vector<double> res;
        for(size_t i = 1; i < m_Points.size(); ++i)
        {
            res.push_back(m_Points[i].x - m_Points[i - 1].x);
        }
        return res;
    }

    std::vector<double> SPChipInterpolation2D::calculateDeltas() const
    {
        std::vector<double> res;
        for(size_t i = 1; i < m_Points.size(); ++i)
        {
            res.push_back((m_Points[i].y - m_Points[i - 1].y)
                          / (m_Points[i].x - m_Points[i - 1].x));
        }
        return res;
    }

    std::vector<double> SPChipInterpolation2D::calculateDerivatives() const
    {
        std::vector<double> res;
        // first get the special cases, first and last
        auto first_res =
          ((2 * m_Hs[0] + m_Hs[1]) * m_Deltas[0] - (m_Hs[0] * m_Deltas[1])) / (m_Hs[0] + m_Hs[1]);
        if(sgn(first_res) != sgn(m_Deltas[0]))
        {
            first_res = 0;
        }
        else if((sgn(m_Deltas[0]) != sgn(m_Deltas[1]))
                && (std::abs(first_res) > std::abs(3 * m_Deltas[0])))
        {
            first_res = 3 * m_Deltas[0];
        }
        auto last_h = *(m_Hs.end() - 1);
        auto penultimate_h = *(m_Hs.end() - 2);
        auto last_d = *(m_Deltas.end() - 1);
        auto penultimate_d = *(m_Deltas.end() - 2);

        auto last_res = ((2 * last_h + penultimate_h) * last_d - last_h * penultimate_d)
                        / (last_h + penultimate_h);

        if(sgn(last_res) != sgn(last_d))
        {
            last_res = 0;
        }
        else if((sgn(last_d) != sgn(penultimate_d)) && (std::abs(last_res) > std::abs(3 * last_d)))
        {
            last_res = 3 * last_d;
        }

        res.push_back(first_res);
        for(size_t i = 1; i < m_Hs.size(); ++i)
        {
            res.push_back(
              piecewiseCubicDerivative(m_Deltas[i], m_Deltas[i - 1], m_Hs[i], m_Hs[i - 1]));
        }

        res.push_back(last_res);
        return res;
    }

    double SPChipInterpolation2D::piecewiseCubicDerivative(double const delta_k,
                                                            double const delta_k_minus_1,
                                                            double const hk,
                                                            double const hk_minus_1)
    {
        double res;
        if((delta_k == 0) || (delta_k_minus_1 == 0) || (delta_k > 0 && delta_k_minus_1 < 0)
           || (delta_k < 0 && delta_k_minus_1 > 0))
        {
            return 0;
        }
        if(hk == hk_minus_1)
        {
            res = .5 * (1 / delta_k_minus_1 + 1 / delta_k);
            res = 1 / res;
        }
        else
        {
            auto w1 = 2 * hk + hk_minus_1;
            auto w2 = hk + 2 * hk_minus_1;
            res = (w1 / delta_k_minus_1) + (w2 / delta_k);
            res = (w1 + w2) / res;
        }
        return res;
    }

    double SPChipInterpolation2D::interpolate(double const h,
                                               double const s,
                                               double const y_k,
                                               double const y_k_plus_one,
                                               double const d_k,
                                               double const d_k_plus_one) const
    {
        return ((3 * h * std::pow(s, 2) - 2 * std::pow(s, 3)) / std::pow(h, 3)) * y_k_plus_one
               + ((std::pow(h, 3) - 3 * h * std::pow(s, 2) + 2 * std::pow(s, 3)) / std::pow(h, 3))
                   * y_k
               + ((std::pow(s, 2) * (s - h)) / std::pow(h, 2)) * d_k_plus_one
               + ((s * std::pow(s - h, 2)) / std::pow(h, 2)) * d_k;
    }
}   // namespace FenestrationCommon
