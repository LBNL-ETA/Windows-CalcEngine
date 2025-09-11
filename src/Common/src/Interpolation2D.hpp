#pragma once

#include <vector>

namespace FenestrationCommon
{
    struct TableValue
    {
        TableValue(double t_X, double t_Y) : x(t_X), y(t_Y)
        {}
        double x;
        double y;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    // SPChipInterpolation2D
    //////////////////////////////////////////////////////////////////////////////////////

    class SPChipInterpolation2D
    {
    public:
        explicit SPChipInterpolation2D(const std::vector<TableValue> & t_Points);

        [[nodiscard]] double value(double t_Value) const;

    private:
        [[nodiscard]] std::size_t getSubinterval(double t_Value) const;
        [[nodiscard]] std::vector<double> calculateHs() const;
        [[nodiscard]] std::vector<double> calculateDeltas() const;
        [[nodiscard]] std::vector<double> calculateDerivatives() const;
        static double piecewiseCubicDerivative(double delta_k,
                                               double delta_k_minus_1,
                                               double hk,
                                               double hk_minus_1);
        [[nodiscard]] double interpolate(double h,
                                         double s,
                                         double y_k,
                                         double y_k_plus_one,
                                         double d_k,
                                         double d_k_plus_one) const;

        std::vector<double> m_Hs;
        std::vector<double> m_Deltas;
        std::vector<double> m_Derivatives;
        std::vector<TableValue> m_Points;
    };

}   // namespace FenestrationCommon
