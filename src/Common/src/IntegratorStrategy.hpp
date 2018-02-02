#ifndef INTEGRATIONSTRATEGY_H
#define INTEGRATIONSTRATEGY_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

	enum class IntegrationType {
		Rectangular, RectangularCentroid, Trapezoidal, TrapezoidalA, TrapezoidalB
	};

	class ISeriesPoint;

	class CSeries;

	class IIntegratorStrategy {
	public:
		virtual ~IIntegratorStrategy() = default;

		// virtual double integrate( double const x1, double const x2, double const y1, double const y2 ) = 0;
		virtual std::unique_ptr< CSeries >
		integrate( const std::vector< std::unique_ptr< ISeriesPoint > > & t_Series ) = 0;
	protected:
		double dX( double x1, double x2 ) const;
	};

	class CIntegratorRectangular : public IIntegratorStrategy {
	public:
		std::unique_ptr< CSeries >
		integrate( const std::vector< std::unique_ptr< ISeriesPoint > > & t_Series ) override;
	};

	class CIntegratorRectangularCentroid : public IIntegratorStrategy {
	public:
		std::unique_ptr< CSeries >
		integrate( const std::vector< std::unique_ptr< ISeriesPoint > > & t_Series ) override;
	};

	class CIntegratorTrapezoidal : public IIntegratorStrategy {
	public:
		std::unique_ptr< CSeries >
		integrate( const std::vector< std::unique_ptr< ISeriesPoint > > & t_Series ) override;
	};

	class CIntegratorTrapezoidalA : public IIntegratorStrategy {
	public:
		std::unique_ptr< CSeries >
		integrate( const std::vector< std::unique_ptr< ISeriesPoint > > & t_Series ) override;
	};

	class CIntegratorTrapezoidalB : public IIntegratorStrategy {
	public:
		std::unique_ptr< CSeries >
		integrate( const std::vector< std::unique_ptr< ISeriesPoint > > & t_Series ) override;
	};

	class CIntegratorFactory {
	public:
		std::shared_ptr< IIntegratorStrategy > getIntegrator( IntegrationType t_IntegratorType ) const;
	};

}

#endif
