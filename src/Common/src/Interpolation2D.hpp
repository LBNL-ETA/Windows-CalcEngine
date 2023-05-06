#pragma once

#include <vector>

namespace FenestrationCommon
{
    //////////////////////////////////////////////////////////////////////////////////////
    // IInterpolation2D
    //////////////////////////////////////////////////////////////////////////////////////

    // Interface for 2D interpolation curve
    class IInterpolation2D
    {
    public:
        explicit IInterpolation2D(std::vector<std::pair<double, double>> const & t_Points);

        [[nodiscard]] virtual double getValue(double t_Value) const = 0;

    protected:
        std::vector<std::pair<double, double>> m_Points;
    };

    //////////////////////////////////////////////////////////////////////////////////////
    // CSPChipInterpolation2D
    //////////////////////////////////////////////////////////////////////////////////////

    class CSPChipInterpolation2D : public IInterpolation2D
    {
    public:
        explicit CSPChipInterpolation2D(std::vector<std::pair<double, double>> const & t_Points);

        [[nodiscard]] double getValue(double t_Value) const;

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
    };

}   // namespace FenestrationCommon
