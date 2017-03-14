#include <memory>
#include <algorithm>
#include <gtest/gtest.h>

#include "SpectralSample.hpp"
#include "MeasuredSampleData.hpp"
#include "AngularSpectralSample.hpp"
#include "Series.hpp"
#include "IntegratorStrategy.hpp"
#include "SurfaceCoating.hpp"
#include "FenestrationCommon.hpp"
#include "AngularMeasurements.hpp"

using namespace std;
using namespace SpectralAveraging;
using namespace FenestrationCommon;

// Example (test case) of sample that calculates angular properties of single layer sample with linear interpolation

class TestSampleNFRC_103_AngularTest: public testing::Test {

public:
	std::vector< std::shared_ptr< CSingleAngularMeasurement > > m_Measurements;

private:
//  shared_ptr< CAngularSpectralSample > m_Sample;

protected:
  virtual void SetUp() {
    shared_ptr< CSeries >  aSolarRadiation = make_shared< CSeries >();
    
    // Full ASTM E891-87 Table 1
    aSolarRadiation->addProperty( 0.3000, 0.0    );
    aSolarRadiation->addProperty( 0.3050, 3.4    );
    aSolarRadiation->addProperty( 0.3100, 15.6   );
    aSolarRadiation->addProperty( 0.3150, 41.1   );
    aSolarRadiation->addProperty( 0.3200, 71.2   );
    aSolarRadiation->addProperty( 0.3250, 100.2  );
    aSolarRadiation->addProperty( 0.3300, 152.4  );
    aSolarRadiation->addProperty( 0.3350, 155.6  );
    aSolarRadiation->addProperty( 0.3400, 179.4  );
    aSolarRadiation->addProperty( 0.3450, 186.7  );
    aSolarRadiation->addProperty( 0.3500, 212.0  );
    aSolarRadiation->addProperty( 0.3600, 240.5  );
    aSolarRadiation->addProperty( 0.3700, 324.0  );
    aSolarRadiation->addProperty( 0.3800, 362.4  );
    aSolarRadiation->addProperty( 0.3900, 381.7  );
    aSolarRadiation->addProperty( 0.4000, 556.0  );
    aSolarRadiation->addProperty( 0.4100, 656.3  );
    aSolarRadiation->addProperty( 0.4200, 690.8  );
    aSolarRadiation->addProperty( 0.4300, 641.9  );
    aSolarRadiation->addProperty( 0.4400, 798.5  );
    aSolarRadiation->addProperty( 0.4500, 956.6  );
    aSolarRadiation->addProperty( 0.4600, 990.0  );
    aSolarRadiation->addProperty( 0.4700, 998.0  );
    aSolarRadiation->addProperty( 0.4800, 1046.1 );
    aSolarRadiation->addProperty( 0.4900, 1005.1 );
    aSolarRadiation->addProperty( 0.5000, 1026.7 );
    aSolarRadiation->addProperty( 0.5100, 1066.7 );
    aSolarRadiation->addProperty( 0.5200, 1011.5 );
    aSolarRadiation->addProperty( 0.5300, 1084.9 );
    aSolarRadiation->addProperty( 0.5400, 1082.4 );
    aSolarRadiation->addProperty( 0.5500, 1102.2 );
    aSolarRadiation->addProperty( 0.5700, 1087.4 );
    aSolarRadiation->addProperty( 0.5900, 1024.3 );
    aSolarRadiation->addProperty( 0.6100, 1088.8 );
    aSolarRadiation->addProperty( 0.6300, 1062.1 );
    aSolarRadiation->addProperty( 0.6500, 1061.7 );
    aSolarRadiation->addProperty( 0.6700, 1046.2 );
    aSolarRadiation->addProperty( 0.6900, 859.2  );
    aSolarRadiation->addProperty( 0.7100, 1002.4 );
    aSolarRadiation->addProperty( 0.7180, 816.9  );
    aSolarRadiation->addProperty( 0.7244, 842.8  );
    aSolarRadiation->addProperty( 0.7400, 971.0  );
    aSolarRadiation->addProperty( 0.7525, 956.3  );
    aSolarRadiation->addProperty( 0.7575, 942.2  );
    aSolarRadiation->addProperty( 0.7625, 524.8  );
    aSolarRadiation->addProperty( 0.7675, 830.7  );
    aSolarRadiation->addProperty( 0.7800, 908.9  );
    aSolarRadiation->addProperty( 0.8000, 873.4  );
    aSolarRadiation->addProperty( 0.8160, 712.0  );
    aSolarRadiation->addProperty( 0.8237, 660.2  );
    aSolarRadiation->addProperty( 0.8315, 765.5  );
    aSolarRadiation->addProperty( 0.8400, 799.8  );
    aSolarRadiation->addProperty( 0.8600, 815.2  );
    aSolarRadiation->addProperty( 0.8800, 778.3  );
    aSolarRadiation->addProperty( 0.9050, 630.4  );
    aSolarRadiation->addProperty( 0.9150, 565.2  );
    aSolarRadiation->addProperty( 0.9250, 586.4  );
    aSolarRadiation->addProperty( 0.9300, 348.1  );
    aSolarRadiation->addProperty( 0.9370, 224.2  );
    aSolarRadiation->addProperty( 0.9480, 271.4  );
    aSolarRadiation->addProperty( 0.9650, 451.2  );
    aSolarRadiation->addProperty( 0.9800, 549.7  );
    aSolarRadiation->addProperty( 0.9935, 630.1  );
    aSolarRadiation->addProperty( 1.0400, 582.9  );
    aSolarRadiation->addProperty( 1.0700, 539.7  );
    aSolarRadiation->addProperty( 1.1000, 366.2  );
    aSolarRadiation->addProperty( 1.1200, 98.1   );
    aSolarRadiation->addProperty( 1.1300, 169.5  );
    aSolarRadiation->addProperty( 1.1370, 118.7  );
    aSolarRadiation->addProperty( 1.1610, 301.9  );
    aSolarRadiation->addProperty( 1.1800, 406.8  );
    aSolarRadiation->addProperty( 1.2000, 375.2  );
    aSolarRadiation->addProperty( 1.2350, 423.6  );
    aSolarRadiation->addProperty( 1.2900, 365.7  );
    aSolarRadiation->addProperty( 1.3200, 223.4  );
    aSolarRadiation->addProperty( 1.3500, 30.1   );
    aSolarRadiation->addProperty( 1.3950, 1.4    );
    aSolarRadiation->addProperty( 1.4425, 51.6   );
    aSolarRadiation->addProperty( 1.4625, 97.0   );
    aSolarRadiation->addProperty( 1.4770, 97.3   );
    aSolarRadiation->addProperty( 1.4970, 167.1  );
    aSolarRadiation->addProperty( 1.5200, 239.3  );
    aSolarRadiation->addProperty( 1.5390, 248.8  );
    aSolarRadiation->addProperty( 1.5580, 249.3  );
    aSolarRadiation->addProperty( 1.5780, 222.3  );
    aSolarRadiation->addProperty( 1.5920, 227.3  );
    aSolarRadiation->addProperty( 1.6100, 210.5  );
    aSolarRadiation->addProperty( 1.6300, 224.7  );
    aSolarRadiation->addProperty( 1.6460, 215.9  );
    aSolarRadiation->addProperty( 1.6780, 202.8  );
    aSolarRadiation->addProperty( 1.7400, 158.2  );
    aSolarRadiation->addProperty( 1.8000, 28.6   );
    aSolarRadiation->addProperty( 1.8600, 1.8    );
    aSolarRadiation->addProperty( 1.9200, 1.1    );
    aSolarRadiation->addProperty( 1.9600, 19.7   );
    aSolarRadiation->addProperty( 1.9850, 84.9   );
    aSolarRadiation->addProperty( 2.0050, 25.0   );
    aSolarRadiation->addProperty( 2.0350, 92.5   );
    aSolarRadiation->addProperty( 2.0650, 56.3   );
    aSolarRadiation->addProperty( 2.1000, 82.7   );
    aSolarRadiation->addProperty( 2.1480, 76.2   );
    aSolarRadiation->addProperty( 2.1980, 66.4   );
    aSolarRadiation->addProperty( 2.2700, 65.0   );
    aSolarRadiation->addProperty( 2.3600, 57.6   );
    aSolarRadiation->addProperty( 2.4500, 19.8   );
    aSolarRadiation->addProperty( 2.4940, 17.0   );
    aSolarRadiation->addProperty( 2.5370, 3.0    );
    aSolarRadiation->addProperty( 2.9410, 4.0    );
    aSolarRadiation->addProperty( 2.9730, 7.0    );
    aSolarRadiation->addProperty( 3.0050, 6.0    );
    aSolarRadiation->addProperty( 3.0560, 3.0    );
    aSolarRadiation->addProperty( 3.1320, 5.0    );
    aSolarRadiation->addProperty( 3.1560, 18.0   );
    aSolarRadiation->addProperty( 3.2040, 1.2    );
    aSolarRadiation->addProperty( 3.2450, 3.0    );
    aSolarRadiation->addProperty( 3.3170, 12.0   );
    aSolarRadiation->addProperty( 3.3440, 3.0    );
    aSolarRadiation->addProperty( 3.4500, 12.2   );
    aSolarRadiation->addProperty( 3.5730, 11.0   );
    aSolarRadiation->addProperty( 3.7650, 9.0    );
    aSolarRadiation->addProperty( 4.0450, 6.9    );

	shared_ptr< CSpectralSampleData > aMeasurements4 = make_shared< CSpectralSampleData >( );
	// incident angle = 40

	aMeasurements4->addRecord( 0.290, 0, 0.1, 0.1 );
	aMeasurements4->addRecord( 0.295, 0, 0.099, 0.099 );
	aMeasurements4->addRecord( 0.300, 0, 0.098, 0.098 );
	aMeasurements4->addRecord( 0.305, 0.1, 0.098, 0.098 );

	shared_ptr< CSpectralSample > aSample4 = make_shared< CSpectralSample >( aMeasurements4, aSolarRadiation );

	shared_ptr< CSingleAngularMeasurement > aAngular4 = make_shared< CSingleAngularMeasurement >( aSample4, 40 );

	shared_ptr< CSpectralSampleData > aMeasurements5 = make_shared< CSpectralSampleData >( );
	// incident angle = 50

	aMeasurements5->addRecord( 0.290, 0, 0.90, 0.80 );
	aMeasurements5->addRecord( 0.295, 0, 0.099, 0.099 );
	aMeasurements5->addRecord( 0.300, 0, 0.0986, 0.0986 );
	aMeasurements5->addRecord( 0.305, 0.2, 0.0984, 0.0984 );

	shared_ptr< CSpectralSample > aSample5 = make_shared< CSpectralSample >( aMeasurements5, aSolarRadiation );

	shared_ptr< CSingleAngularMeasurement > aAngular5 = make_shared< CSingleAngularMeasurement >( aSample5, 50 );

	m_Measurements.push_back( aAngular4 );
	m_Measurements.push_back( aAngular5 );

//	CCommonWavelengths aCommonWL;
	std::shared_ptr< std::vector< double > > wl = aSample4->getWavelengthsFromSample( );
//	aCommonWL.addWavelength( aSample4->getWavelengthsFromSample( ) );
//	aCommonWL.addWavelength( aSample4->getWavelengthsFromSample( ) );
//	aCommonWL.addWavelength( aSample5->getBandWavelengths( ) );
//	shared_ptr< vector< double > > commonWavelengths = aCommonWL.getCombinedWavelengths( Combine::Interpolate );

  }

};

TEST_F( TestSampleNFRC_103_AngularTest, TestProperties45degrees ) {

  double angle = 45;

  shared_ptr< CAngularMeasurements > angularMeasurement = make_shared< CAngularMeasurements >( m_Measurements );

  shared_ptr< CSingleAngularMeasurement > aAngular = angularMeasurement->getMeasurements( angle );

  shared_ptr< CSpectralSample > aSample = aAngular->getData( );

  std::shared_ptr< FenestrationCommon::CSeries > trans = aSample->getMeasuredData( )->properties( SampleData::T );;

  double transmittance = ( *trans )[3]->value( );
  EXPECT_NEAR( 0.15, transmittance, 1e-6 );

  std::shared_ptr< FenestrationCommon::CSeries > reflef = aSample->getMeasuredData( )->properties( SampleData::Rf );;

  double reflectanceFront = ( *reflef )[3]->value( );
  EXPECT_NEAR( 0.0982, reflectanceFront, 1e-6 );

  std::shared_ptr< FenestrationCommon::CSeries > refleb = aSample->getMeasuredData( )->properties( SampleData::Rb );;

  double reflectanceBack = ( *refleb )[3]->value( );
  EXPECT_NEAR( 0.0982, reflectanceBack, 1e-6 );

  // SOLAR RANGE
  double lowLambda = 0.3;
  double highLambda = 2.5;

  transmittance = aSample->getProperty( lowLambda, highLambda, Property::T, Side::Front );
  reflectanceFront = aSample->getProperty( lowLambda, highLambda, Property::R, Side::Front );
  reflectanceBack = aSample->getProperty( lowLambda, highLambda, Property::R, Side::Back );


}

