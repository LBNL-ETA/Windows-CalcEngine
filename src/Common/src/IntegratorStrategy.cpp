#include <cassert>

#include "IntegratorStrategy.hpp"
#include "Series.hpp"

namespace FenestrationCommon
{
    double delta(double x1, double x2) { return x2 - x1; }

    // ---- Small local helper: iterate adjacent pairs once ----
    template<class Compute>
    static CSeries integrate_pairs(const std::vector<CSeriesPoint>& s,
                                   double normalizationCoeff,
                                   Compute&& compute)
    {
        CSeries out;
        if (s.size() < 2) return out;
        assert(normalizationCoeff != 0.0 && "normalizationCoeff must be non-zero");

        const auto n = s.size();
        for (std::size_t i = 1; i < n; ++i)
        {
            const auto& L = s[i - 1];
            const auto& R = s[i];

            const double w1 = L.x();
            const double w2 = R.x();
            const double y1 = L.value();
            const double y2 = R.value();
            const double dx = delta(w1, w2);

            const double contrib = compute(i, n, y1, y2, dx);
            out.addProperty(w1, contrib / normalizationCoeff);
        }
        return out;
    }

    // Left-rectangle rule
    CSeries CIntegratorRectangular::integrate(const std::vector<CSeriesPoint>& s,
                                              double normalizationCoeff)
    {
        return integrate_pairs(s, normalizationCoeff,
            [](std::size_t /*i*/, std::size_t /*n*/,
               double y1, double /*y2*/, double dx)
            {
                return y1 * dx;
            });
    }

    // Rectangular (centroid). With delta(x - d, x' - d) == x' - x, this
    // reduces to the same contribution as the left-rectangle rule.
    CSeries CIntegratorRectangularCentroid::integrate(const std::vector<CSeriesPoint>& s,
                                                      double normalizationCoeff)
    {
        return integrate_pairs(s, normalizationCoeff,
            [](std::size_t /*i*/, std::size_t /*n*/,
               double y1, double /*y2*/, double dx)
            {
                return y1 * dx; // same as above given delta() = x2 - x1
            });
    }

    // Standard trapezoid
    CSeries CIntegratorTrapezoidal::integrate(const std::vector<CSeriesPoint>& s,
                                              double normalizationCoeff)
    {
        return integrate_pairs(s, normalizationCoeff,
            [](std::size_t /*i*/, std::size_t /*n*/,
               double y1, double y2, double dx)
            {
                const double yMid = (y1 + y2) * 0.5;
                return yMid * dx;
            });
    }

    // TrapezoidalA: add half-endpoint contributions to the first/last segment
    CSeries CIntegratorTrapezoidalA::integrate(const std::vector<CSeriesPoint>& s,
                                               double normalizationCoeff)
    {
        return integrate_pairs(s, normalizationCoeff,
            [](std::size_t i, std::size_t n,
               double y1, double y2, double dx)
            {
                double val = ((y1 + y2) * 0.5) * dx;
                if (i == 1)       val += (y1 * 0.5) * dx;
                if (i == n - 1)   val += (y2 * 0.5) * dx;
                return val;
            });
    }

    // TrapezoidalB: add quarter-endpoint contributions to both first/last segments
    CSeries CIntegratorTrapezoidalB::integrate(const std::vector<CSeriesPoint>& s,
                                               double normalizationCoeff)
    {
        return integrate_pairs(s, normalizationCoeff,
            [](std::size_t i, std::size_t n,
               double y1, double y2, double dx)
            {
                double val = ((y1 + y2) * 0.5) * dx;
                if (i == 1 || i == n - 1)
                    val += ((y1 + y2) * 0.25) * dx;
                return val;
            });
    }

    // Pre-weighted (keeps original behavior)
    CSeries CIntegratorPreWeighted::integrate(const std::vector<CSeriesPoint>& s,
                                              double normalizationCoeff)
    {
        CSeries out;
        assert(normalizationCoeff != 0.0 && "normalizationCoeff must be non-zero");
        for (std::size_t i = 0; i < s.size(); ++i)
        {
            const double y = s[i].value();
            out.addProperty(1, y / normalizationCoeff);
        }
        return out;
    }

    std::unique_ptr<IIntegratorStrategy>
    CIntegratorFactory::getIntegrator(IntegrationType type) const
    {
        std::unique_ptr<IIntegratorStrategy> p = nullptr;
        switch (type)
        {
            case IntegrationType::Rectangular:         p = std::make_unique<CIntegratorRectangular>(); break;
            case IntegrationType::RectangularCentroid: p = std::make_unique<CIntegratorRectangularCentroid>(); break;
            case IntegrationType::Trapezoidal:         p = std::make_unique<CIntegratorTrapezoidal>(); break;
            case IntegrationType::TrapezoidalA:        p = std::make_unique<CIntegratorTrapezoidalA>(); break;
            case IntegrationType::TrapezoidalB:        p = std::make_unique<CIntegratorTrapezoidalB>(); break;
            case IntegrationType::PreWeighted:         p = std::make_unique<CIntegratorPreWeighted>(); break;
            default: assert(!"Irregular call of integration strategy."); break;
        }
        return p;
    }
} // namespace FenestrationCommon
