#include <memory>
#include <gtest/gtest.h>

#include "EquivalentBSDFLayer.hpp"
#include "SpectralSample.hpp"
#include "Series.hpp"
#include "SpecularLayer.hpp"
#include "PerforatedCellDescription.hpp"
#include "MultiPaneSampleData.hpp"
#include "MeasuredSampleData.hpp"
#include "MaterialDescription.hpp"
#include "FenestrationCommon.hpp"
#include "BSDFDirections.hpp"
#include "SquareMatrix.hpp"
#include "BSDFLayer.hpp"
#include "BSDFLayerMaker.hpp"

using namespace std;
using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example on how to create multilayer BSDF with combination of specular and perforated layer

class EquivalentBSDFLayer_102_Perforated : public testing::Test {

private:
  shared_ptr< CEquivalentBSDFLayer > m_Layer;

  shared_ptr< CSeries > loadSolarRadiationFile() {

    shared_ptr< CSeries >  aSolarRadiation = make_shared< CSeries >();

    // Full ASTM E891-87 Table 1 (Solar radiation)
    aSolarRadiation->addProperty( 0.3000, 0.0 );
    aSolarRadiation->addProperty( 0.3050, 3.4 );
    aSolarRadiation->addProperty( 0.3100, 15.6 );
    aSolarRadiation->addProperty( 0.3150, 41.1 );
    aSolarRadiation->addProperty( 0.3200, 71.2 );
    aSolarRadiation->addProperty( 0.3250, 100.2 );
    aSolarRadiation->addProperty( 0.3300, 152.4 );
    aSolarRadiation->addProperty( 0.3350, 155.6 );
    aSolarRadiation->addProperty( 0.3400, 179.4 );
    aSolarRadiation->addProperty( 0.3450, 186.7 );
    aSolarRadiation->addProperty( 0.3500, 212.0 );
    aSolarRadiation->addProperty( 0.3600, 240.5 );
    aSolarRadiation->addProperty( 0.3700, 324.0 );
    aSolarRadiation->addProperty( 0.3800, 362.4 );
    aSolarRadiation->addProperty( 0.3900, 381.7 );
    aSolarRadiation->addProperty( 0.4000, 556.0 );
    aSolarRadiation->addProperty( 0.4100, 656.3 );
    aSolarRadiation->addProperty( 0.4200, 690.8 );
    aSolarRadiation->addProperty( 0.4300, 641.9 );
    aSolarRadiation->addProperty( 0.4400, 798.5 );
    aSolarRadiation->addProperty( 0.4500, 956.6 );
    aSolarRadiation->addProperty( 0.4600, 990.0 );
    aSolarRadiation->addProperty( 0.4700, 998.0 );
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
    aSolarRadiation->addProperty( 0.6900, 859.2 );
    aSolarRadiation->addProperty( 0.7100, 1002.4 );
    aSolarRadiation->addProperty( 0.7180, 816.9 );
    aSolarRadiation->addProperty( 0.7244, 842.8 );
    aSolarRadiation->addProperty( 0.7400, 971.0 );
    aSolarRadiation->addProperty( 0.7525, 956.3 );
    aSolarRadiation->addProperty( 0.7575, 942.2 );
    aSolarRadiation->addProperty( 0.7625, 524.8 );
    aSolarRadiation->addProperty( 0.7675, 830.7 );
    aSolarRadiation->addProperty( 0.7800, 908.9 );
    aSolarRadiation->addProperty( 0.8000, 873.4 );
    aSolarRadiation->addProperty( 0.8160, 712.0 );
    aSolarRadiation->addProperty( 0.8237, 660.2 );
    aSolarRadiation->addProperty( 0.8315, 765.5 );
    aSolarRadiation->addProperty( 0.8400, 799.8 );
    aSolarRadiation->addProperty( 0.8600, 815.2 );
    aSolarRadiation->addProperty( 0.8800, 778.3 );
    aSolarRadiation->addProperty( 0.9050, 630.4 );
    aSolarRadiation->addProperty( 0.9150, 565.2 );
    aSolarRadiation->addProperty( 0.9250, 586.4 );
    aSolarRadiation->addProperty( 0.9300, 348.1 );
    aSolarRadiation->addProperty( 0.9370, 224.2 );
    aSolarRadiation->addProperty( 0.9480, 271.4 );
    aSolarRadiation->addProperty( 0.9650, 451.2 );
    aSolarRadiation->addProperty( 0.9800, 549.7 );
    aSolarRadiation->addProperty( 0.9935, 630.1 );
    aSolarRadiation->addProperty( 1.0400, 582.9 );
    aSolarRadiation->addProperty( 1.0700, 539.7 );
    aSolarRadiation->addProperty( 1.1000, 366.2 );
    aSolarRadiation->addProperty( 1.1200, 98.1 );
    aSolarRadiation->addProperty( 1.1300, 169.5 );
    aSolarRadiation->addProperty( 1.1370, 118.7 );
    aSolarRadiation->addProperty( 1.1610, 301.9 );
    aSolarRadiation->addProperty( 1.1800, 406.8 );
    aSolarRadiation->addProperty( 1.2000, 375.2 );
    aSolarRadiation->addProperty( 1.2350, 423.6 );
    aSolarRadiation->addProperty( 1.2900, 365.7 );
    aSolarRadiation->addProperty( 1.3200, 223.4 );
    aSolarRadiation->addProperty( 1.3500, 30.1 );
    aSolarRadiation->addProperty( 1.3950, 1.4 );
    aSolarRadiation->addProperty( 1.4425, 51.6 );
    aSolarRadiation->addProperty( 1.4625, 97.0 );
    aSolarRadiation->addProperty( 1.4770, 97.3 );
    aSolarRadiation->addProperty( 1.4970, 167.1 );
    aSolarRadiation->addProperty( 1.5200, 239.3 );
    aSolarRadiation->addProperty( 1.5390, 248.8 );
    aSolarRadiation->addProperty( 1.5580, 249.3 );
    aSolarRadiation->addProperty( 1.5780, 222.3 );
    aSolarRadiation->addProperty( 1.5920, 227.3 );
    aSolarRadiation->addProperty( 1.6100, 210.5 );
    aSolarRadiation->addProperty( 1.6300, 224.7 );
    aSolarRadiation->addProperty( 1.6460, 215.9 );
    aSolarRadiation->addProperty( 1.6780, 202.8 );
    aSolarRadiation->addProperty( 1.7400, 158.2 );
    aSolarRadiation->addProperty( 1.8000, 28.6 );
    aSolarRadiation->addProperty( 1.8600, 1.8 );
    aSolarRadiation->addProperty( 1.9200, 1.1 );
    aSolarRadiation->addProperty( 1.9600, 19.7 );
    aSolarRadiation->addProperty( 1.9850, 84.9 );
    aSolarRadiation->addProperty( 2.0050, 25.0 );
    aSolarRadiation->addProperty( 2.0350, 92.5 );
    aSolarRadiation->addProperty( 2.0650, 56.3 );
    aSolarRadiation->addProperty( 2.1000, 82.7 );
    aSolarRadiation->addProperty( 2.1480, 76.2 );
    aSolarRadiation->addProperty( 2.1980, 66.4 );
    aSolarRadiation->addProperty( 2.2700, 65.0 );
    aSolarRadiation->addProperty( 2.3600, 57.6 );
    aSolarRadiation->addProperty( 2.4500, 19.8 );
    aSolarRadiation->addProperty( 2.4940, 17.0 );
    aSolarRadiation->addProperty( 2.5370, 3.0 );
    aSolarRadiation->addProperty( 2.9410, 4.0 );
    aSolarRadiation->addProperty( 2.9730, 7.0 );
    aSolarRadiation->addProperty( 3.0050, 6.0 );
    aSolarRadiation->addProperty( 3.0560, 3.0 );
    aSolarRadiation->addProperty( 3.1320, 5.0 );
    aSolarRadiation->addProperty( 3.1560, 18.0 );
    aSolarRadiation->addProperty( 3.2040, 1.2 );
    aSolarRadiation->addProperty( 3.2450, 3.0 );
    aSolarRadiation->addProperty( 3.3170, 12.0 );
    aSolarRadiation->addProperty( 3.3440, 3.0 );
    aSolarRadiation->addProperty( 3.4500, 12.2 );
    aSolarRadiation->addProperty( 3.5730, 11.0 );
    aSolarRadiation->addProperty( 3.7650, 9.0 );
    aSolarRadiation->addProperty( 4.0450, 6.9 );

    return aSolarRadiation;
  }

  shared_ptr< CSpectralSampleData > loadSampleData_NFRC_102() {
    shared_ptr< CSpectralSampleData > aMeasurements_102 = make_shared< CSpectralSampleData >();

    aMeasurements_102->addRecord( 0.300, 0.0020, 0.0470, 0.0480 );
    aMeasurements_102->addRecord( 0.305, 0.0030, 0.0470, 0.0480 );
    aMeasurements_102->addRecord( 0.310, 0.0090, 0.0470, 0.0480 );
    aMeasurements_102->addRecord( 0.315, 0.0350, 0.0470, 0.0480 );
    aMeasurements_102->addRecord( 0.320, 0.1000, 0.0470, 0.0480 );
    aMeasurements_102->addRecord( 0.325, 0.2180, 0.0490, 0.0500 );
    aMeasurements_102->addRecord( 0.330, 0.3560, 0.0530, 0.0540 );
    aMeasurements_102->addRecord( 0.335, 0.4980, 0.0600, 0.0610 );
    aMeasurements_102->addRecord( 0.340, 0.6160, 0.0670, 0.0670 );
    aMeasurements_102->addRecord( 0.345, 0.7090, 0.0730, 0.0740 );
    aMeasurements_102->addRecord( 0.350, 0.7740, 0.0780, 0.0790 );
    aMeasurements_102->addRecord( 0.355, 0.8180, 0.0820, 0.0820 );
    aMeasurements_102->addRecord( 0.360, 0.8470, 0.0840, 0.0840 );
    aMeasurements_102->addRecord( 0.365, 0.8630, 0.0850, 0.0850 );
    aMeasurements_102->addRecord( 0.370, 0.8690, 0.0850, 0.0860 );
    aMeasurements_102->addRecord( 0.375, 0.8610, 0.0850, 0.0850 );
    aMeasurements_102->addRecord( 0.380, 0.8560, 0.0840, 0.0840 );
    aMeasurements_102->addRecord( 0.385, 0.8660, 0.0850, 0.0850 );
    aMeasurements_102->addRecord( 0.390, 0.8810, 0.0860, 0.0860 );
    aMeasurements_102->addRecord( 0.395, 0.8890, 0.0860, 0.0860 );
    aMeasurements_102->addRecord( 0.400, 0.8930, 0.0860, 0.0860 );
    aMeasurements_102->addRecord( 0.410, 0.8930, 0.0860, 0.0860 );
    aMeasurements_102->addRecord( 0.420, 0.8920, 0.0860, 0.0860 );
    aMeasurements_102->addRecord( 0.430, 0.8920, 0.0850, 0.0850 );
    aMeasurements_102->addRecord( 0.440, 0.8920, 0.0850, 0.0850 );
    aMeasurements_102->addRecord( 0.450, 0.8960, 0.0850, 0.0850 );
    aMeasurements_102->addRecord( 0.460, 0.9000, 0.0850, 0.0850 );
    aMeasurements_102->addRecord( 0.470, 0.9020, 0.0840, 0.0840 );
    aMeasurements_102->addRecord( 0.480, 0.9030, 0.0840, 0.0840 );
    aMeasurements_102->addRecord( 0.490, 0.9040, 0.0850, 0.0850 );
    aMeasurements_102->addRecord( 0.500, 0.9050, 0.0840, 0.0840 );
    aMeasurements_102->addRecord( 0.510, 0.9050, 0.0840, 0.0840 );
    aMeasurements_102->addRecord( 0.520, 0.9050, 0.0840, 0.0840 );
    aMeasurements_102->addRecord( 0.530, 0.9040, 0.0840, 0.0840 );
    aMeasurements_102->addRecord( 0.540, 0.9040, 0.0830, 0.0830 );
    aMeasurements_102->addRecord( 0.550, 0.9030, 0.0830, 0.0830 );
    aMeasurements_102->addRecord( 0.560, 0.9020, 0.0830, 0.0830 );
    aMeasurements_102->addRecord( 0.570, 0.9000, 0.0820, 0.0820 );
    aMeasurements_102->addRecord( 0.580, 0.8980, 0.0820, 0.0820 );
    aMeasurements_102->addRecord( 0.590, 0.8960, 0.0810, 0.0810 );
    aMeasurements_102->addRecord( 0.600, 0.8930, 0.0810, 0.0810 );
    aMeasurements_102->addRecord( 0.610, 0.8900, 0.0810, 0.0810 );
    aMeasurements_102->addRecord( 0.620, 0.8860, 0.0800, 0.0800 );
    aMeasurements_102->addRecord( 0.630, 0.8830, 0.0800, 0.0800 );
    aMeasurements_102->addRecord( 0.640, 0.8790, 0.0790, 0.0790 );
    aMeasurements_102->addRecord( 0.650, 0.8750, 0.0790, 0.0790 );
    aMeasurements_102->addRecord( 0.660, 0.8720, 0.0790, 0.0790 );
    aMeasurements_102->addRecord( 0.670, 0.8680, 0.0780, 0.0780 );
    aMeasurements_102->addRecord( 0.680, 0.8630, 0.0780, 0.0780 );
    aMeasurements_102->addRecord( 0.690, 0.8590, 0.0770, 0.0770 );
    aMeasurements_102->addRecord( 0.700, 0.8540, 0.0760, 0.0770 );
    aMeasurements_102->addRecord( 0.710, 0.8500, 0.0760, 0.0760 );
    aMeasurements_102->addRecord( 0.720, 0.8450, 0.0750, 0.0760 );
    aMeasurements_102->addRecord( 0.730, 0.8400, 0.0750, 0.0750 );
    aMeasurements_102->addRecord( 0.740, 0.8350, 0.0750, 0.0750 );
    aMeasurements_102->addRecord( 0.750, 0.8310, 0.0740, 0.0740 );
    aMeasurements_102->addRecord( 0.760, 0.8260, 0.0740, 0.0740 );
    aMeasurements_102->addRecord( 0.770, 0.8210, 0.0740, 0.0740 );
    aMeasurements_102->addRecord( 0.780, 0.8160, 0.0730, 0.0730 );
    aMeasurements_102->addRecord( 0.790, 0.8120, 0.0730, 0.0730 );
    aMeasurements_102->addRecord( 0.800, 0.8080, 0.0720, 0.0720 );
    aMeasurements_102->addRecord( 0.810, 0.8030, 0.0720, 0.0720 );
    aMeasurements_102->addRecord( 0.820, 0.8000, 0.0720, 0.0720 );
    aMeasurements_102->addRecord( 0.830, 0.7960, 0.0710, 0.0710 );
    aMeasurements_102->addRecord( 0.840, 0.7930, 0.0700, 0.0710 );
    aMeasurements_102->addRecord( 0.850, 0.7880, 0.0700, 0.0710 );
    aMeasurements_102->addRecord( 0.860, 0.7860, 0.0700, 0.0700 );
    aMeasurements_102->addRecord( 0.870, 0.7820, 0.0740, 0.0740 );
    aMeasurements_102->addRecord( 0.880, 0.7800, 0.0720, 0.0720 );
    aMeasurements_102->addRecord( 0.890, 0.7770, 0.0730, 0.0740 );
    aMeasurements_102->addRecord( 0.900, 0.7760, 0.0720, 0.0720 );
    aMeasurements_102->addRecord( 0.910, 0.7730, 0.0720, 0.0720 );
    aMeasurements_102->addRecord( 0.920, 0.7710, 0.0710, 0.0710 );
    aMeasurements_102->addRecord( 0.930, 0.7700, 0.0700, 0.0700 );
    aMeasurements_102->addRecord( 0.940, 0.7680, 0.0690, 0.0690 );
    aMeasurements_102->addRecord( 0.950, 0.7660, 0.0680, 0.0680 );
    aMeasurements_102->addRecord( 0.960, 0.7660, 0.0670, 0.0680 );
    aMeasurements_102->addRecord( 0.970, 0.7640, 0.0680, 0.0680 );
    aMeasurements_102->addRecord( 0.980, 0.7630, 0.0680, 0.0680 );
    aMeasurements_102->addRecord( 0.990, 0.7620, 0.0670, 0.0670 );
    aMeasurements_102->addRecord( 1.000, 0.7620, 0.0660, 0.0670 );
    aMeasurements_102->addRecord( 1.050, 0.7600, 0.0660, 0.0660 );
    aMeasurements_102->addRecord( 1.100, 0.7590, 0.0660, 0.0660 );
    aMeasurements_102->addRecord( 1.150, 0.7610, 0.0660, 0.0660 );
    aMeasurements_102->addRecord( 1.200, 0.7650, 0.0660, 0.0660 );
    aMeasurements_102->addRecord( 1.250, 0.7700, 0.0650, 0.0650 );
    aMeasurements_102->addRecord( 1.300, 0.7770, 0.0670, 0.0670 );
    aMeasurements_102->addRecord( 1.350, 0.7860, 0.0660, 0.0670 );
    aMeasurements_102->addRecord( 1.400, 0.7950, 0.0670, 0.0680 );
    aMeasurements_102->addRecord( 1.450, 0.8080, 0.0670, 0.0670 );
    aMeasurements_102->addRecord( 1.500, 0.8190, 0.0690, 0.0690 );
    aMeasurements_102->addRecord( 1.550, 0.8290, 0.0690, 0.0690 );
    aMeasurements_102->addRecord( 1.600, 0.8360, 0.0700, 0.0700 );
    aMeasurements_102->addRecord( 1.650, 0.8400, 0.0700, 0.0700 );
    aMeasurements_102->addRecord( 1.700, 0.8420, 0.0690, 0.0700 );
    aMeasurements_102->addRecord( 1.750, 0.8420, 0.0690, 0.0700 );
    aMeasurements_102->addRecord( 1.800, 0.8410, 0.0700, 0.0700 );
    aMeasurements_102->addRecord( 1.850, 0.8400, 0.0690, 0.0690 );
    aMeasurements_102->addRecord( 1.900, 0.8390, 0.0680, 0.0680 );
    aMeasurements_102->addRecord( 1.950, 0.8390, 0.0710, 0.0710 );
    aMeasurements_102->addRecord( 2.000, 0.8390, 0.0690, 0.0690 );
    aMeasurements_102->addRecord( 2.050, 0.8400, 0.0680, 0.0680 );
    aMeasurements_102->addRecord( 2.100, 0.8410, 0.0680, 0.0680 );
    aMeasurements_102->addRecord( 2.150, 0.8390, 0.0690, 0.0690 );
    aMeasurements_102->addRecord( 2.200, 0.8300, 0.0700, 0.0700 );
    aMeasurements_102->addRecord( 2.250, 0.8300, 0.0700, 0.0700 );
    aMeasurements_102->addRecord( 2.300, 0.8320, 0.0690, 0.0690 );
    aMeasurements_102->addRecord( 2.350, 0.8320, 0.0690, 0.0700 );
    aMeasurements_102->addRecord( 2.400, 0.8320, 0.0700, 0.0700 );
    aMeasurements_102->addRecord( 2.450, 0.8260, 0.0690, 0.0690 );
    aMeasurements_102->addRecord( 2.500, 0.8220, 0.0680, 0.0680 );

    return aMeasurements_102;

  }

protected:
  virtual void SetUp() {
    shared_ptr< CSeries >  aSolarRadiation = loadSolarRadiationFile();

    shared_ptr< CSpectralSampleData > aMeasurements_102 = loadSampleData_NFRC_102();

    shared_ptr< CSpectralSample > aSample_102 = make_shared< CSpectralSample >( aMeasurements_102 );

    double thickness = 3.048e-3; // [m]
    shared_ptr< CMaterial > aMaterial_102 = 
      make_shared< CMaterialSample >( aSample_102, thickness, MaterialType::Monolithic, WavelengthRange::Solar );

    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );
    shared_ptr< CBSDFLayer > Layer_102 = CBSDFLayerMaker( aMaterial_102, aBSDF ).getLayer();

    // Setting circular perforated shade with double range material
    double Tmat = 0.1;
    double Rfmat = 0.7;
    double Rbmat = 0.7;
    shared_ptr< CMaterial > aSolarRangeMaterial = 
      make_shared< CMaterialSingleBand >( Tmat, Tmat, Rfmat, Rbmat, WavelengthRange::Solar );

    // Visible range
    Tmat = 0.2;
    Rfmat = 0.6;
    Rbmat = 0.6;
    shared_ptr< CMaterial > aVisibleRangeMaterial = 
      make_shared< CMaterialSingleBand >( Tmat, Tmat, Rfmat, Rbmat, WavelengthRange::Visible );

    // double ratio = 0.49;

    shared_ptr< CMaterial > aMaterialPerforated = 
      make_shared< CMaterialDualBand >( aVisibleRangeMaterial, aSolarRangeMaterial );

    // make cell geometry
    double x = 19.05; // mm
    double y = 19.05; // mm
    thickness = 5; // mm
    double radius = 3.175; // mm
    shared_ptr< CCellDescription > aCellDescription = 
      make_shared< CCircularCellDescription >( x, y, thickness, radius );

    // Perforated layer is created here
    shared_ptr< CBSDFLayer > Layer_Perforated = 
      CBSDFLayerMaker( aMaterialPerforated, aBSDF, aCellDescription ).getLayer();

    shared_ptr< vector< double > > commonWavelengths = Layer_102->getBandWavelengths();

    // Equivalent multilayer
    m_Layer = make_shared< CEquivalentBSDFLayer >( commonWavelengths, aSolarRadiation, Layer_102 );
    m_Layer->addLayer( Layer_Perforated );

  }

public:
  shared_ptr< CEquivalentBSDFLayer > getLayer() { return m_Layer; };

};

TEST_F( EquivalentBSDFLayer_102_Perforated, Test102Perofrated1 ) {
  SCOPED_TRACE( "Begin Test: Specular layer - BSDF." );

  const double minLambda = 0.3;
  const double maxLambda = 2.5;
  
  CEquivalentBSDFLayer aLayer = *getLayer();

  double tauDiff = aLayer.DiffDiff( minLambda, maxLambda, Side::Front, PropertySimple::T );
  EXPECT_NEAR( 0.1063500, tauDiff, 1e-6 );

  double rhoDiff = aLayer.DiffDiff( minLambda, maxLambda, Side::Front, PropertySimple::R );
  EXPECT_NEAR( 0.5703955, rhoDiff, 1e-6 );

  double absDiff1 = aLayer.AbsDiff( minLambda, maxLambda, Side::Front, 1 );
  EXPECT_NEAR( 0.1595164, absDiff1, 1e-6 );

  double absDiff2 = aLayer.AbsDiff( minLambda, maxLambda, Side::Front, 2 );
  EXPECT_NEAR( 0.1615728, absDiff2, 1e-6 );

  double theta = 0;
  double phi = 0;

  double tauHem = aLayer.DirHem( minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi );
  EXPECT_NEAR( 0.1619052, tauHem, 1e-6 );

  double tauDir = aLayer.DirDir( minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi );
  EXPECT_NEAR( 0.0748575, tauDir, 1e-6 );

  double rhoHem = aLayer.DirHem( minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi );
  EXPECT_NEAR( 0.5134613, rhoHem, 1e-6 );

  double rhoDir = aLayer.DirDir( minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi );
  EXPECT_NEAR( 0.0866742, rhoDir, 1e-6 );

  double abs1 = aLayer.Abs( minLambda, maxLambda, Side::Front, 1, theta, phi );
  EXPECT_NEAR( 0.1527933, abs1, 1e-6 );

  double abs2 = aLayer.Abs( minLambda, maxLambda, Side::Front, 2, theta, phi );
  EXPECT_NEAR( 0.1696748, abs2, 1e-6 );

  theta = 45;
  phi = 78;

  tauHem = aLayer.DirHem( minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi );
  EXPECT_NEAR( 0.1239460, tauHem, 1e-6 );

  tauDir = aLayer.DirDir( minLambda, maxLambda, Side::Front, PropertySimple::T, theta, phi );
  EXPECT_NEAR( 0.0329390, tauDir, 1e-6 );

  rhoHem = aLayer.DirHem( minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi );
  EXPECT_NEAR( 0.5354738, rhoHem, 1e-6 );

  rhoDir = aLayer.DirDir( minLambda, maxLambda, Side::Front, PropertySimple::R, theta, phi );
  EXPECT_NEAR( 0.0915399, rhoDir, 1e-6 );

  abs1 = aLayer.Abs( minLambda, maxLambda, Side::Front, 1, theta, phi );
  EXPECT_NEAR( 0.1619522, abs1, 1e-6 );

  abs2 = aLayer.Abs( minLambda, maxLambda, Side::Front, 2, theta, phi );
  EXPECT_NEAR( 0.1764627, abs2, 1e-6 );

  CSquareMatrix aT = *aLayer.getMatrix( minLambda, maxLambda, Side::Front, PropertySimple::T );

  // Front transmittance matrix
  size_t size = aT.getSize();

  vector< double > correctResults;
  correctResults.push_back( 0.973690205548513490 );
  correctResults.push_back( 0.785391770476540850 );
  correctResults.push_back( 0.785391770476540850 );
  correctResults.push_back( 0.785391770476540850 );
  correctResults.push_back( 0.785391770476540850 );
  correctResults.push_back( 0.785391770476540850 );
  correctResults.push_back( 0.785391770476540850 );
  correctResults.push_back( 0.785391770476540850 );
  correctResults.push_back( 0.785391770476540850 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.428107513510513350 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.029147056442274891 );
  correctResults.push_back( 0.018794977675071475 );
  correctResults.push_back( 0.018794977675071475 );
  correctResults.push_back( 0.018794977675071475 );
  correctResults.push_back( 0.018794977675071475 );
  correctResults.push_back( 0.018794977675071475 );
  correctResults.push_back( 0.018794977675071475 );
  correctResults.push_back( 0.018794977675071475 );
  correctResults.push_back( 0.018794977675071475 );

  EXPECT_EQ( correctResults.size(), aT.getSize() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aT[ i ][ i ], 1e-6 );
  }

  // Back Reflectance matrix
  CSquareMatrix aRb = *aLayer.getMatrix( minLambda, maxLambda, Side::Back, PropertySimple::R );
  
  correctResults.clear();
  
  correctResults.push_back( 0.21190787064540886 );
  correctResults.push_back( 0.21381058993245247 );
  correctResults.push_back( 0.21381058993245247 );
  correctResults.push_back( 0.21381058993245247 );
  correctResults.push_back( 0.21381058993245247 );
  correctResults.push_back( 0.21381058993245247 );
  correctResults.push_back( 0.21381058993245247 );
  correctResults.push_back( 0.21381058993245247 );
  correctResults.push_back( 0.21381058993245247 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.21685764259086368 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  correctResults.push_back( 0.22355023727552928 );
  
  EXPECT_EQ( correctResults.size(), aRb.getSize() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aRb[ i ][ i ], 1e-6 );
  }

  // Front absorptance layer 1
  vector< double > aAbsF = *aLayer.Abs( minLambda, maxLambda, Side::Front, 1 );

  correctResults.clear();
  
  correctResults.push_back( 0.15279332457211262 );
  correctResults.push_back( 0.15586972860833670 );
  correctResults.push_back( 0.15586972860833670 );
  correctResults.push_back( 0.15586972860833670 );
  correctResults.push_back( 0.15586972860833670 );
  correctResults.push_back( 0.15586972860833670 );
  correctResults.push_back( 0.15586972860833670 );
  correctResults.push_back( 0.15586972860833670 );
  correctResults.push_back( 0.15586972860833670 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16195217141408391 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.16830288659050252 );
  correctResults.push_back( 0.14752614735016453 );
  correctResults.push_back( 0.14752614735016453 );
  correctResults.push_back( 0.14752614735016453 );
  correctResults.push_back( 0.14752614735016453 );
  correctResults.push_back( 0.14752614735016453 );
  correctResults.push_back( 0.14752614735016453 );
  correctResults.push_back( 0.14752614735016453 );
  correctResults.push_back( 0.14752614735016453 );
  
  EXPECT_EQ( correctResults.size(), aAbsF.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aAbsF[ i ], 1e-6 );
  }

  // Front absorptance layer 2
  aAbsF = *aLayer.Abs( minLambda, maxLambda, Side::Front, 2 );

  correctResults.clear();
  
  correctResults.push_back( 0.16967476787077485 );
  correctResults.push_back( 0.17342216546987113 );
  correctResults.push_back( 0.17342216546987113 );
  correctResults.push_back( 0.17342216546987116 );
  correctResults.push_back( 0.17342216546987116 );
  correctResults.push_back( 0.17342216546987116 );
  correctResults.push_back( 0.17342216546987116 );
  correctResults.push_back( 0.17342216546987116 );
  correctResults.push_back( 0.17342216546987116 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17646265577042369 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.17434881317737461 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.17434881317737461 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.17434881317737466 );
  correctResults.push_back( 0.11071889341880466 );
  correctResults.push_back( 0.11071889341880466 );
  correctResults.push_back( 0.11071889341880466 );
  correctResults.push_back( 0.11071889341880468 );
  correctResults.push_back( 0.11071889341880466 );
  correctResults.push_back( 0.11071889341880466 );
  correctResults.push_back( 0.11071889341880468 );
  correctResults.push_back( 0.11071889341880468 );
  
  EXPECT_EQ( correctResults.size(), aAbsF.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aAbsF[ i ], 1e-6 );
  }

  // Back absorptance layer 1
  vector< double > aAbsB = *aLayer.Abs( minLambda, maxLambda, Side::Back, 1 );
  
  correctResults.clear();
  
  correctResults.push_back( 0.0128042767120028940 );
  correctResults.push_back( 0.0108812889619819970 );
  correctResults.push_back( 0.0108812889619819950 );
  correctResults.push_back( 0.0108812889619819950 );
  correctResults.push_back( 0.0108812889619819970 );
  correctResults.push_back( 0.0108812889619819970 );
  correctResults.push_back( 0.0108812889619819970 );
  correctResults.push_back( 0.0108812889619819970 );
  correctResults.push_back( 0.0108812889619819950 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0085094151944131412 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  correctResults.push_back( 0.0047981817861313426 );
  
  EXPECT_EQ( correctResults.size(), aAbsB.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aAbsB[ i ], 1e-6 );    
  }

  // Back absorptance layer 2
  aAbsB = *aLayer.Abs( minLambda, maxLambda, Side::Back, 2 );
  
  correctResults.clear();
  
  correctResults.push_back( 0.18698593111387821 );
  correctResults.push_back( 0.19121254358929551 );
  correctResults.push_back( 0.19121254358929551 );
  correctResults.push_back( 0.19121254358929551 );
  correctResults.push_back( 0.19121254358929551 );
  correctResults.push_back( 0.19121254358929551 );
  correctResults.push_back( 0.19121254358929551 );
  correctResults.push_back( 0.19121254358929551 );
  correctResults.push_back( 0.19121254358929551 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.19644660737657524 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  correctResults.push_back( 0.20340472745929269 );
  
  EXPECT_EQ( correctResults.size(), aAbsB.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aAbsB[ i ], 1e-6 );
  }

}