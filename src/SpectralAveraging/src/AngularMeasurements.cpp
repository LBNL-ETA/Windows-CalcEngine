#include <stdexcept>
#include <assert.h>
#include <algorithm>
#include <math.h>

#include "SpectralSample.hpp"
#include "MeasuredSampleData.hpp"
#include "Series.hpp"
#include "IntegratorStrategy.hpp"
#include "FenestrationCommon.hpp"
#include "AngularMeasurements.hpp"

using namespace std;
using namespace FenestrationCommon;

namespace SpectralAveraging {

  //////////////////////////////////////////////////////////////////////////////////////
  ////  AngularMeasurement
  //////////////////////////////////////////////////////////////////////////////////////

	CSingleAngularMeasurement::CSingleAngularMeasurement( const std::shared_ptr< CSpectralSample > t_Data, const double t_Angle ) : m_Data( t_Data ), m_Angle( t_Angle ) {
		if (t_Data == nullptr) {
			throw runtime_error( "Sample must have measured data in AngularMeasurement." );
		}

	}

	double CSingleAngularMeasurement::getAngle( ) const {
		return m_Angle;
	}

	shared_ptr< CSpectralSample > CSingleAngularMeasurement::getData( ) const {
		return m_Data;
	}

	shared_ptr< CSpectralSample > CSingleAngularMeasurement::Interpolate( const double t_Angle, const std::shared_ptr< CSpectralSample > t_Data1, const double t_Angle1, const std::shared_ptr< CSpectralSample > t_Data2, const double t_Angle2 ) const
	{

		shared_ptr< CSpectralSampleData > aData = make_shared< CSpectralSampleData >( );
		std::shared_ptr< std::vector< double > > wlv = t_Data1->getWavelengthsFromSample( );
		std::shared_ptr< FenestrationCommon::CSeries > trans1 = t_Data1->getMeasuredData( )->properties( SampleData::T );;
		std::shared_ptr< FenestrationCommon::CSeries > trans2 = t_Data2->getMeasuredData( )->properties( SampleData::T );;
		std::shared_ptr< FenestrationCommon::CSeries > reflef1 = t_Data1->getMeasuredData( )->properties( SampleData::Rf );;
		std::shared_ptr< FenestrationCommon::CSeries > reflef2 = t_Data2->getMeasuredData( )->properties( SampleData::Rf );;
		std::shared_ptr< FenestrationCommon::CSeries > refleb1 = t_Data1->getMeasuredData( )->properties( SampleData::Rb );;
		std::shared_ptr< FenestrationCommon::CSeries > refleb2 = t_Data2->getMeasuredData( )->properties( SampleData::Rb );;
		double frac = ( t_Angle - t_Angle1 ) / ( t_Angle2 - t_Angle1 );
		double wl;
		double t1, t2, t;
		double rf1, rf2, rf;
		double rb1, rb2, rb;
		int size = int(t_Data1->getBandSize( ));
		for (int i = 0; i < size; i++) {
			wl = (*wlv)[i];
			t1 = ( *trans1 )[i]->value( );
			t2 = ( *trans2 )[i]->value( );
			rf1 = ( *reflef1 )[i]->value( );
			rf2 = ( *reflef2 )[i]->value( );
			rb1 = ( *refleb1 )[i]->value( );
			rb2 = ( *refleb2 )[i]->value( );
			t = t1 + frac * ( t2 - t1 );
			rf = rf1 + frac * ( rf2 - rf1 );
			rb = rb1 + frac * ( rb2 - rb1 );
			aData->addRecord( wl, t, rf, rb );
		}
		shared_ptr< CSpectralSample > aSample = make_shared< CSpectralSample >( aData, t_Data1->getSourceData( ) );

		return aSample;
	}

	void CSingleAngularMeasurement::interpolate( const vector< double >& t_CommonWavelengths ) {
		m_Data->getMeasuredData( )->interpolate( t_CommonWavelengths );
	}

	//////////////////////////////////////////////////////////////////////////////////////
	////  CAngularMeasurements
	//////////////////////////////////////////////////////////////////////////////////////

	CAngularMeasurements::CAngularMeasurements( std::shared_ptr< CSingleAngularMeasurement > t_SignleMeasurement, const std::shared_ptr< std::vector< double > > t_CommonWavelengths ):
		m_SingleMeasurement( t_SignleMeasurement ), m_CommonWavelengths( t_CommonWavelengths )
	{
		if (m_SingleMeasurement == nullptr) {
			throw runtime_error( "Sample must have measured data in AngularMeasurements." );
		}
	}
	CAngularMeasurements::CAngularMeasurements( std::vector< std::shared_ptr< CSingleAngularMeasurement > > t_Measurements ): m_Measurements( t_Measurements )  {

	}

	// then you add function that will accept measurements at different angles
	void CAngularMeasurements::addMeasurement( std::shared_ptr< CSingleAngularMeasurement > t_SingleMeasurement )
	{
		t_SingleMeasurement->interpolate( *m_CommonWavelengths );
		m_Measurements.push_back( t_SingleMeasurement );
	}

	// Now insert very important function here
	std::shared_ptr< CSingleAngularMeasurement > CAngularMeasurements::getMeasurements( const double t_Angle )
	{
		const double angleTolerance = 1e-6;

		for (int i = 0; i < m_Measurements.size( ); i++ ) {
			if (fabs( m_Measurements[i]->getAngle( ) - t_Angle ) < angleTolerance ) {
				return m_Measurements[i];
			}
		}

		double min1 = 100000;
		double min2 = 100000;
		double angle1 = 0.0;
		double angle2 = 0.0;
		double angle;
		int imin = 0;
		int imax = 0;
		double diff;
		std::shared_ptr< CSpectralSample > sample1 = nullptr;
		std::shared_ptr< CSpectralSample > sample2 = nullptr;
		for (int i = 0; i < m_Measurements.size( ); i++) {
			angle = m_Measurements[i]->getAngle( );
			diff = fabs( angle - t_Angle );
			if ( diff < min1 ) {
				sample1 = m_Measurements[i]->getData();
				angle1 = angle;
				min1 = diff;
				imin = i;
			}
			else if (diff < min2) {
				sample2 = m_Measurements[i]->getData();
				angle2 = angle;
				min2 = diff;
				imax = i;
			}
		}

		// create sample3 (CSpectralSample) that is linear interploation between sample1 and sample2
		// 1. Get out SourceData
		// 2. Get out SpecralSampleData
		// 3. Do interpolation between two SpectralSampleData and create third SpectralSampleData
		// 4. Create SpectralSample from third SpectralSampleData and given SourceData

		std::shared_ptr< SpectralAveraging::CSpectralSample > sample3 = nullptr;
		sample3 = m_Measurements[imax]->Interpolate( t_Angle, sample1, angle1, sample2, angle2 );

		shared_ptr< CSingleAngularMeasurement > aAngular = make_shared< CSingleAngularMeasurement >( sample3, t_Angle );
		m_Measurements.push_back( aAngular );
		return aAngular;
	}

}
