#include <cmath>

#include "SpectrumFunctions.hpp"
#include "WCECommon.hpp"

std::vector< std::pair< double, double > >
SpectralAveraging::UVAction( const std::vector< double > & t_data, double a, double b ) {
	std::vector< std::pair< double, double > > result;
	for ( auto & val: t_data ) {
		double value = std::exp( a - b * val );
		result.emplace_back( val, value );
	}

	return result;
}

std::vector< std::pair< double, double > >
SpectralAveraging::Krochmann( const std::vector< double > & t_data ) {
	return UVAction( t_data, 12.28, 25.56 );
}

std::vector< std::pair< double, double > >
SpectralAveraging::BlackBodySpectrum( const std::vector< double > & t_data, double t_temperature ) {
	std::vector< std::pair< double, double > > result;
	double firstRadiationConstant = 2 * ConstantsData::PI * ConstantsData::PLANKCONSTANT *
							std::pow( ConstantsData::SPEEDOFLIGHT, 2 );
	double secondRadiationConstant =
			ConstantsData::PLANKCONSTANT * ConstantsData::SPEEDOFLIGHT / ConstantsData::BOLTZMANNCONSTANT;

	for ( auto & val: t_data ) {
		double wlInMeters = val * 1e-6;
		double energy = firstRadiationConstant / std::pow( wlInMeters, 5 ) * 1 /
													 ( std::exp( secondRadiationConstant / ( wlInMeters * t_temperature ) ) - 1 );
		result.emplace_back( val, energy );
	}

	return result;
}
