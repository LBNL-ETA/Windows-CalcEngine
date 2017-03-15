#ifndef ANGULARMEASUREMENT_H
#define ANGULARMEASUREMENT_H

#include <memory>
#include <vector>

namespace FenestrationCommon {

	enum class Property;
	enum class Side;
	enum class IntegrationType;
	class CSeries;

}

namespace SpectralAveraging {

	class CSpectralSampleData;
	class CSpectralAngleSample;
	class CAngularSpectralProperties;

//	enum class WavelengthSet { Custom, Source, Data };

	// Base class for spectral sample data. Its base setup are spectral properties over certain range. It handles detector and source data.
	// Concrete sample data are handled in inherited classes and tha will depend on type of the sample data
	// First class for single measurement at certain angle
	class CSingleAngularMeasurement {
	public:
		CSingleAngularMeasurement( const std::shared_ptr< CSpectralSample > t_Data, const double t_Angle );

		double getAngle( ) const;
		std::shared_ptr< CSpectralSample > getData( ) const;
    std::shared_ptr< std::vector< double > > getWavelengthsFromSample() const;
		std::shared_ptr< CSpectralSample > Interpolate( const double t_Angle, const std::shared_ptr< CSpectralSample > t_Data1, const double t_Angle1, const std::shared_ptr< CSpectralSample > t_Data2, const double t_Angle2 ) const;
		void interpolate( const std::vector< double >& t_Wavelengths );
//		std::shared_ptr< CSpectralSample > Interpolate( const double t_fraction, const std::shared_ptr< CSpectralSample > t_Data1, const std::shared_ptr< CSpectralSample > t_Data2 ) const;

	private:
		std::shared_ptr< CSpectralSample > m_Data;
		double m_Angle;
	};

	// Note that name here is plural
	class CAngularMeasurements {
	public:
		// You want to provide different measurement and also you want to provide common wavelengths (set of points for interpolation)
		CAngularMeasurements( std::shared_ptr< CSingleAngularMeasurement > t_SingleMeasurement, const std::shared_ptr<
			std::vector< double > > t_CommonWavelengths );
		CAngularMeasurements( std::vector< std::shared_ptr< CSingleAngularMeasurement > > t_Measurements );
		// then you add function that will accept measurements at different angles
		void addMeasurement( std::shared_ptr< CSingleAngularMeasurement > t_SingleMeasurement );
		// Now insert very important function here
		std::shared_ptr< CSingleAngularMeasurement > getMeasurements( const double t_Angle );
		// Note that previous function (getMeasurements) needs to do interpolation if user asks for t_Angle that does not exist.So this is where you want to do your interpolation
			// work
	private:
		// Do not forget storage for it
		std::shared_ptr< CSingleAngularMeasurement > m_SingleMeasurement;
		std::vector< std::shared_ptr< CSingleAngularMeasurement > > m_Measurements;
		std::shared_ptr< std::vector< double > > m_CommonWavelengths;
		std::shared_ptr< CSingleAngularMeasurement > m_Angle;

	}; 

}


#endif