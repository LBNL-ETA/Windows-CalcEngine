#pragma once

#include <memory>
#include <vector>

#include "Series.hpp"


namespace FenestrationCommon
{
    enum class IntegrationType
    {
        Rectangular,
        RectangularCentroid,
        Trapezoidal,
        TrapezoidalA,
        TrapezoidalB,
        PreWeighted
    };

    class ISeriesPoint;

    class CSeries;

    double delta(double x1, double x2);

    class IIntegratorStrategy
    {
    public:
        virtual ~IIntegratorStrategy() = default;
        
        virtual CSeries integrate(const std::vector<CSeriesPoint> & t_Series,
                                  double normalizationCoeff = 1) = 0;
    };

    class CIntegratorRectangular : public IIntegratorStrategy
    {
    public:
        CSeries integrate(const std::vector<CSeriesPoint> & t_Series,
                          double normalizationCoeff) override;
    };

    class CIntegratorRectangularCentroid : public IIntegratorStrategy
    {
    public:
        CSeries integrate(const std::vector<CSeriesPoint> & t_Series,
                          double normalizationCoeff) override;
    };

    class CIntegratorTrapezoidal : public IIntegratorStrategy
    {
    public:
        CSeries integrate(const std::vector<CSeriesPoint> & t_Series,
                          double normalizationCoeff) override;
    };

    class CIntegratorTrapezoidalA : public IIntegratorStrategy
    {
    public:
        CSeries integrate(const std::vector<CSeriesPoint> & t_Series,
                          double normalizationCoeff) override;
    };

    class CIntegratorTrapezoidalB : public IIntegratorStrategy
    {
    public:
        CSeries integrate(const std::vector<CSeriesPoint> & t_Series,
                          double normalizationCoeff) override;
    };

    class CIntegratorPreWeighted : public IIntegratorStrategy
    {
    public:
        CSeries integrate(const std::vector<CSeriesPoint> & t_Series,
                          double normalizationCoeff) override;
    };

    class CIntegratorFactory
    {
    public:
        std::unique_ptr<IIntegratorStrategy> getIntegrator(IntegrationType t_IntegratorType) const;
    };

}   // namespace FenestrationCommon
