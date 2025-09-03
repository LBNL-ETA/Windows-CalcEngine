#include <cassert>
#include "IntegratorStrategy.hpp"
#include "Series.hpp"

namespace FenestrationCommon
{

    static inline double delta(double x1, double x2)
    {
        return x2 - x1;
    }

    // Walk adjacent pairs; store contribution at the left x (w1).
    // Works with any Series-like that has size() and operator[] returning CSeriesPoint.
    template<class SeriesLike, class Compute>
    static CSeries
      integrate_pairs(const SeriesLike & s, double normalizationCoeff, Compute && compute)
    {
        CSeries out;
        const std::size_t n = s.size();
        if(n < 2)
            return out;
        assert(normalizationCoeff != 0.0 && "normalizationCoeff must be non-zero");

        for(std::size_t i = 1; i < n; ++i)
        {
            const CSeriesPoint & L = s[i - 1];
            const CSeriesPoint & R = s[i];

            const double w1 = L.x();
            const double dx = delta(w1, R.x());
            const double y1 = L.value();
            const double y2 = R.value();

            const bool isFirst = (i == 1);
            const bool isLast = (i == n - 1);

            const double contrib = compute(y1, y2, dx, isFirst, isLast);
            out.addProperty(w1, contrib / normalizationCoeff);
        }
        return out;
    }

    CSeries integrate(IntegrationType type, const CSeries & series, double normalizationCoeff)
    {
        switch(type)
        {
            case IntegrationType::Rectangular:
            case IntegrationType::RectangularCentroid:   // same with dx = x2 - x1
                return integrate_pairs(
                  series,
                  normalizationCoeff,
                  [](double y1, double /*y2*/, double dx, bool /*f*/, bool /*l*/) {
                      return y1 * dx;
                  });

            case IntegrationType::Trapezoidal:
                return integrate_pairs(series,
                                       normalizationCoeff,
                                       [](double y1, double y2, double dx, bool /*f*/, bool /*l*/) {
                                           return 0.5 * (y1 + y2) * dx;
                                       });

            case IntegrationType::TrapezoidalA:
                return integrate_pairs(series,
                                       normalizationCoeff,
                                       [](double y1, double y2, double dx, bool first, bool last) {
                                           double v = 0.5 * (y1 + y2) * dx;
                                           if(first)
                                               v += 0.5 * y1 * dx;
                                           if(last)
                                               v += 0.5 * y2 * dx;
                                           return v;
                                       });

            case IntegrationType::TrapezoidalB:
                return integrate_pairs(series,
                                       normalizationCoeff,
                                       [](double y1, double y2, double dx, bool first, bool last) {
                                           double v = 0.5 * (y1 + y2) * dx;
                                           if(first || last)
                                               v += 0.25 * (y1 + y2) * dx;
                                           return v;
                                       });

            case IntegrationType::PreWeighted: {
                CSeries out;
                assert(normalizationCoeff != 0.0 && "normalizationCoeff must be non-zero");
                for(std::size_t i = 0; i < series.size(); ++i)
                {
                    out.addProperty(1.0, series[i].value() / normalizationCoeff);
                }
                return out;
            }
        }

        // To silence warnings; all enum cases covered.
        return CSeries{};
    }

}   // namespace FenestrationCommon
