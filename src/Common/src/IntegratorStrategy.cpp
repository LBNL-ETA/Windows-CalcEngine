#include <cassert>

#include "IntegratorStrategy.hpp"
#include "Series.hpp"

namespace FenestrationCommon
{

    double delta(double x1, double x2)
    {
        return x2 - x1;
    }

    // Walk adjacent pairs; store contribution at the left x (w1).
    // Compute signature: double compute(y1, y2, dx, bool isFirst, bool isLast)
    template<class Compute>
    static CSeries integrate_pairs(const std::vector<CSeriesPoint> & s,
                                   const double normalizationCoeff,
                                   Compute && compute)
    {
        CSeries out;
        if(s.size() < 2)
            return out;
        assert(normalizationCoeff != 0.0 && "normalizationCoeff must be non-zero");

        const auto n{s.size()};
        for(std::size_t i = 1; i < n; ++i)
        {
            const auto & L = s[i - 1];
            const auto & R = s[i];

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

    // ---------------- Integrators ----------------

    CSeries CIntegratorRectangular::integrate(const std::vector<CSeriesPoint> & s,
                                              const double normalizationCoeff)
    {
        return integrate_pairs(
          s,
          normalizationCoeff,
          [](double y1, double /*y2*/, double dx, bool /*first*/, bool /*last*/) {
              return y1 * dx;   // left-rectangle
          });
    }

    CSeries CIntegratorRectangularCentroid::integrate(const std::vector<CSeriesPoint> & s,
                                                      const double normalizationCoeff)
    {
        // With delta = x2 - x1 this reduces to the same as Rectangular
        return integrate_pairs(
          s,
          normalizationCoeff,
          [](double y1, double /*y2*/, double dx, bool /*first*/, bool /*last*/) {
              return y1 * dx;
          });
    }

    CSeries CIntegratorTrapezoidal::integrate(const std::vector<CSeriesPoint> & s,
                                              const double normalizationCoeff)
    {
        return integrate_pairs(s,
                               normalizationCoeff,
                               [](double y1, double y2, double dx, bool /*first*/, bool /*last*/) {
                                   return 0.5 * (y1 + y2) * dx;
                               });
    }

    CSeries CIntegratorTrapezoidalA::integrate(const std::vector<CSeriesPoint> & s,
                                               const double normalizationCoeff)
    {
        return integrate_pairs(
          s, normalizationCoeff, [](double y1, double y2, double dx, bool isFirst, bool isLast) {
              double val = 0.5 * (y1 + y2) * dx;
              if(isFirst)
              {
                  val += 0.5 * y1 * dx;
              }
              if(isLast)
              {
                  val += 0.5 * y2 * dx;
              }
              return val;
          });
    }

    CSeries CIntegratorTrapezoidalB::integrate(const std::vector<CSeriesPoint> & s,
                                               const double normalizationCoeff)
    {
        return integrate_pairs(
          s, normalizationCoeff, [](double y1, double y2, double dx, bool isFirst, bool isLast) {
              double val = 0.5 * (y1 + y2) * dx;
              if(isFirst || isLast)
              {
                  val += 0.25 * (y1 + y2) * dx;
              }
              return val;
          });
    }

    // PreWeighted stays per-point (not pair-based)
    CSeries CIntegratorPreWeighted::integrate(const std::vector<CSeriesPoint> & s,
                                              const double normalizationCoeff)
    {
        CSeries out;
        assert(normalizationCoeff != 0.0 && "normalizationCoeff must be non-zero");
        for(std::size_t i = 0; i < s.size(); ++i)
            out.addProperty(1.0, s[i].value() / normalizationCoeff);
        return out;
    }


    std::unique_ptr<IIntegratorStrategy>
      CIntegratorFactory::getIntegrator(IntegrationType type) const
    {
        std::unique_ptr<IIntegratorStrategy> p = nullptr;
        switch(type)
        {
            case IntegrationType::Rectangular:
                p = std::make_unique<CIntegratorRectangular>();
                break;
            case IntegrationType::RectangularCentroid:
                p = std::make_unique<CIntegratorRectangularCentroid>();
                break;
            case IntegrationType::Trapezoidal:
                p = std::make_unique<CIntegratorTrapezoidal>();
                break;
            case IntegrationType::TrapezoidalA:
                p = std::make_unique<CIntegratorTrapezoidalA>();
                break;
            case IntegrationType::TrapezoidalB:
                p = std::make_unique<CIntegratorTrapezoidalB>();
                break;
            case IntegrationType::PreWeighted:
                p = std::make_unique<CIntegratorPreWeighted>();
                break;
            default:
                assert(!"Irregular call of integration strategy.");
                break;
        }
        return p;
    }
}   // namespace FenestrationCommon
