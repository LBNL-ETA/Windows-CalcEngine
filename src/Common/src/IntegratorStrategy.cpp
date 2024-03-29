#include "IntegratorStrategy.hpp"
#include "Series.hpp"
#include <cassert>

namespace FenestrationCommon
{
    double IIntegratorStrategy::dX(double const x1, double const x2) const
    {
        return x2 - x1;
    }

    CSeries
      CIntegratorRectangular::integrate(const std::vector<CSeriesPoint> & t_Series,
                                        double normalizationCoeff)
    {
        CSeries newProperties;
        for(auto i = 1u; i < t_Series.size(); ++i)
        {
            const auto w1 = t_Series[i - 1].x();
            const auto w2 = t_Series[i].x();
            const auto y1 = t_Series[i - 1].value();
            const auto deltaX = dX(w1, w2);
            const auto value = y1 * deltaX;
            newProperties.addProperty(w1, value / normalizationCoeff);
        }

        return newProperties;
    }

    CSeries CIntegratorRectangularCentroid::integrate(const std::vector<CSeriesPoint> & t_Series, double normalizationCoeff)
    {
        CSeries newProperties;
        for(auto i = 1u; i < t_Series.size(); ++i)
        {
            const auto w1 = t_Series[i - 1].x();
            const auto w2 = t_Series[i].x();
            const auto y1 = t_Series[i - 1].value();
            const auto diffX = (w2 - w1) / 2;
            const auto deltaX = dX(w1 - diffX, w2 - diffX);
            const auto value = y1 * deltaX;
            newProperties.addProperty(w1, value / normalizationCoeff);
        }

        return newProperties;
    }

    CSeries
      CIntegratorTrapezoidal::integrate(const std::vector<CSeriesPoint> & t_Series,
                                        double normalizationCoeff)
    {
        CSeries newProperties;
        for(auto i = 1u; i < t_Series.size(); ++i)
        {
            const auto w1 = t_Series[i - 1].x();
            const auto w2 = t_Series[i].x();
            const auto y1 = t_Series[i - 1].value();
            const auto y2 = t_Series[i].value();
            const auto deltaX = dX(w1, w2);
            const auto yCenter = (y1 + y2) / 2;
            const auto value = yCenter * deltaX;
            newProperties.addProperty(w1, value / normalizationCoeff);
        }

        return newProperties;
    }

    /// TrapezoidalA integration insert additional items before and after first and
    /// last wavelenghts Since WCE is working strictly within wavelengths,
    /// contributions will be added to first and last segment
    CSeries CIntegratorTrapezoidalA::integrate(const std::vector<CSeriesPoint> & t_Series, double normalizationCoeff)
    {
        CSeries newProperties;

        for(auto i = 1u; i < t_Series.size(); ++i)
        {
            const auto w1 = t_Series[i - 1].x();
            const auto w2 = t_Series[i].x();
            const auto y1 = t_Series[i - 1].value();
            const auto y2 = t_Series[i].value();
            const auto deltaX = dX(w1, w2);
            const auto yCenter = (y1 + y2) / 2;
            auto value = yCenter * deltaX;
            if(i == 1)
            {
                value += (y1 / 2) * deltaX;
            }
            if(i == t_Series.size() - 1)
            {
                value += (y2 / 2) * deltaX;
            }
            newProperties.addProperty(w1, value / normalizationCoeff);
        }

        return newProperties;
    }

    CSeries CIntegratorTrapezoidalB::integrate(const std::vector<CSeriesPoint> & t_Series, double normalizationCoeff)
    {
        CSeries newProperties;

        for(auto i = 1u; i < t_Series.size(); ++i)
        {
            const auto w1 = t_Series[i - 1].x();
            const auto w2 = t_Series[i].x();
            const auto y1 = t_Series[i - 1].value();
            const auto y2 = t_Series[i].value();
            const auto deltaX = dX(w1, w2);
            const auto yCenter = (y1 + y2) / 2;
            auto value = yCenter * deltaX;
            if(i == 1 || i == t_Series.size() - 1)
            {
                value += ((y1 + y2) / 4) * deltaX;
            }
            newProperties.addProperty(w1, value / normalizationCoeff);
        }

        return newProperties;
    }

    CSeries
      CIntegratorPreWeighted::integrate(const std::vector<CSeriesPoint> & t_Series,
                                        double normalizationCoeff)
    {
        CSeries newProperties;

        for(auto i = 0u; i < t_Series.size(); ++i)
        {
            const auto y1 = t_Series[i].value();

            newProperties.addProperty(1, y1 / normalizationCoeff);
        }

        return newProperties;
    }

    std::unique_ptr<IIntegratorStrategy>
      CIntegratorFactory::getIntegrator(IntegrationType t_IntegratorType) const
    {
        std::unique_ptr<IIntegratorStrategy> aStrategy = nullptr;
        switch(t_IntegratorType)
        {
            case IntegrationType::Rectangular:
                aStrategy = std::make_unique<CIntegratorRectangular>();
                break;
            case IntegrationType::RectangularCentroid:
                aStrategy = std::make_unique<CIntegratorRectangularCentroid>();
                break;
            case IntegrationType::Trapezoidal:
                aStrategy = std::make_unique<CIntegratorTrapezoidal>();
                break;
            case IntegrationType::TrapezoidalA:
                aStrategy = std::make_unique<CIntegratorTrapezoidalA>();
                break;
            case IntegrationType::TrapezoidalB:
                aStrategy = std::make_unique<CIntegratorTrapezoidalB>();
                break;
            case IntegrationType::PreWeighted:
                aStrategy = std::make_unique<CIntegratorPreWeighted>();
                break;
            default:
                assert("Irregular call of integration strategy.");
                break;
        }
        return aStrategy;
    }
}   // namespace FenestrationCommon
