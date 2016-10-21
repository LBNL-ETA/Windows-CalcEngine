#include <memory>
#include <gtest/gtest.h>

#include "EquivalentBSDFLayer.hpp"
#include "SpectralSample.hpp"
#include "Series.hpp"
#include "SpecularLayer.hpp"
#include "VenetianCellDescription.hpp"
#include "MultiPaneSampleData.hpp"
#include "MeasuredSampleData.hpp"
#include "MaterialDescription.hpp"
#include "CommonWavelengths.hpp"
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

// Example on how to create multilayer BSDF from specular and venetian layers

class EquivalentBSDFLayer_102_VenetianUniform : public testing::Test {

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

    shared_ptr< CSpectralSample > aSample_102 = 
      make_shared< CSpectralSample >( aMeasurements_102 );

    double thickness = 3.048e-3; // [m]
    shared_ptr< CMaterial > aMaterial_102 = 
      make_shared< CMaterialSample >( aSample_102, thickness, MaterialType::Monolithic, WavelengthRange::Solar );

    shared_ptr< CBSDFHemisphere > aBSDF = make_shared< CBSDFHemisphere >( BSDFBasis::Quarter );

    shared_ptr< CBSDFLayer > Layer_102 = CBSDFLayerMaker( aMaterial_102, aBSDF ).getLayer();
    
    // Venetian blind
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

    shared_ptr< CMaterial > aMaterialVenetian = 
      make_shared< CMaterialDualBand >( aVisibleRangeMaterial, aSolarRangeMaterial );

    // make cell geometry
    double slatWidth = 0.016; // m
    double slatSpacing = 0.012; // m
    double slatTiltAngle = 45;
    double curvatureRadius = 0;
    size_t numOfSlatSegments = 5;

    shared_ptr< ICellDescription > aCellDescription = 
      make_shared< CVenetianCellDescription >( slatWidth, slatSpacing, slatTiltAngle, 
      curvatureRadius, numOfSlatSegments );

    shared_ptr< CBSDFLayer > Layer_Venetian = 
      CBSDFLayerMaker( aMaterialVenetian, aBSDF, aCellDescription ).getLayer();

    shared_ptr< vector< double > > commonWavelengths = Layer_102->getBandWavelengths();

    m_Layer = make_shared< CEquivalentBSDFLayer >( commonWavelengths, aSolarRadiation, Layer_102 );
    m_Layer->addLayer( Layer_Venetian );

  }

public:
  shared_ptr< CEquivalentBSDFLayer > getLayer() { return m_Layer; };

};

TEST_F( EquivalentBSDFLayer_102_VenetianUniform, TestBSDF1 ) {
  SCOPED_TRACE( "Begin Test: Specular and venetian uniform IGU - BSDF." );

  const double minLambda = 0.3;
  const double maxLambda = 2.5;
  
  CEquivalentBSDFLayer aLayer = *getLayer();

  CSquareMatrix aT = *aLayer.getMatrix( minLambda, maxLambda, Side::Front, PropertySimple::T );

  // Front transmittance matrix
  size_t size = aT.getSize();

  vector< double > correctResults;
  correctResults.push_back( 0.68057351903170871 );
  correctResults.push_back( 0.72708057561442518 );
  correctResults.push_back( 3.2353295156015958 );
  correctResults.push_back( 4.2740102456696851 );
  correctResults.push_back( 3.2353295156015958 );
  correctResults.push_back( 0.72708057561442496 );
  correctResults.push_back( 0.058762824759963646 );
  correctResults.push_back( 0.0554269217656587 );
  correctResults.push_back( 0.058762824759963632 );
  correctResults.push_back( 0.67071839027302782 );
  correctResults.push_back( 4.3028785408263994 );
  correctResults.push_back( 6.960274584080242 );
  correctResults.push_back( 7.9326257742260129 );
  correctResults.push_back( 6.960274584080242 );
  correctResults.push_back( 4.3028785408263994 );
  correctResults.push_back( 0.67071839027302782 );
  correctResults.push_back( 0.054052878235167717 );
  correctResults.push_back( 0.048836186283178623 );
  correctResults.push_back( 0.046926744505032716 );
  correctResults.push_back( 0.04883618628317863 );
  correctResults.push_back( 0.054052878235167717 );
  correctResults.push_back( 0.63823918064803764 );
  correctResults.push_back( 7.1817901764191445 );
  correctResults.push_back( 8.3290590421460582 );
  correctResults.push_back( 6.5865612587002556 );
  correctResults.push_back( 8.32905904214606 );
  correctResults.push_back( 7.1817901764191445 );
  correctResults.push_back( 0.63823918064803764 );
  correctResults.push_back( 0.045405498681814871 );
  correctResults.push_back( 0.040493627045916671 );
  correctResults.push_back( 0.03900400828620413 );
  correctResults.push_back( 0.040493627045916671 );
  correctResults.push_back( 0.045405498681814871 );
  correctResults.push_back( 0.38844233962217323 );
  correctResults.push_back( 0.058139476563124369 );
  correctResults.push_back( 0.050500645755781219 );
  correctResults.push_back( 0.058139476563124369 );
  correctResults.push_back( 0.38844233962217323 );
  correctResults.push_back( 0.021709980839400955 );
  correctResults.push_back( 0.020073111597617231 );
  correctResults.push_back( 0.021709980839400959 );

  EXPECT_EQ( correctResults.size(), aT.getSize() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aT[ i ][ i ], 1e-6 );
  }

  // Back Reflectance matrix
  CSquareMatrix aRb = *aLayer.getMatrix( minLambda, maxLambda, Side::Back, PropertySimple::R );
  
  correctResults.clear();
  
  correctResults.push_back( 0.14507931635819724 );
  correctResults.push_back( 0.14533404487930526 );
  correctResults.push_back( 0.1607565490432673 );
  correctResults.push_back( 0.16644214119259312 );
  correctResults.push_back( 0.1607565490432673 );
  correctResults.push_back( 0.14533404487930526 );
  correctResults.push_back( 0.11026437791110748 );
  correctResults.push_back( 0.097188794171913187 );
  correctResults.push_back( 0.11026437791110748 );
  correctResults.push_back( 0.14530951151557975 );
  correctResults.push_back( 0.16804145053683514 );
  correctResults.push_back( 0.17809728954408793 );
  correctResults.push_back( 0.18170724929352661 );
  correctResults.push_back( 0.17809728954408793 );
  correctResults.push_back( 0.16804145053683514 );
  correctResults.push_back( 0.14530951151557975 );
  correctResults.push_back( 0.092041709397814378 );
  correctResults.push_back( 0.055461359759175509 );
  correctResults.push_back( 0.042008778027755073 );
  correctResults.push_back( 0.055461359759175509 );
  correctResults.push_back( 0.092041709397814378 );
  correctResults.push_back( 0.14697185645190552 );
  correctResults.push_back( 0.18158019641875806 );
  correctResults.push_back( 0.1909258855446512 );
  correctResults.push_back( 0.1926585311168498 );
  correctResults.push_back( 0.1909258855446512 );
  correctResults.push_back( 0.18158019641875806 );
  correctResults.push_back( 0.14697185645190552 );
  correctResults.push_back( 0.048646943822889137 );
  correctResults.push_back( 0.018713759915000773 );
  correctResults.push_back( 0.030969507974822189 );
  correctResults.push_back( 0.018713759915000773 );
  correctResults.push_back( 0.048646943822889137 );
  correctResults.push_back( 0.16057496907443611 );
  correctResults.push_back( 0.19874127226663674 );
  correctResults.push_back( 0.20368198014218747 );
  correctResults.push_back( 0.19874127226663674 );
  correctResults.push_back( 0.16057496907443611 );
  correctResults.push_back( 0.10004350922018154 );
  correctResults.push_back( 0.11201580167422533 );
  correctResults.push_back( 0.10004350922018154 );
  
  EXPECT_EQ( correctResults.size(), aRb.getSize() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aRb[ i ][ i ], 1e-6 );
  }

  // Front absorptance layer 1
  vector< double > aAbsF = *aLayer.Abs( minLambda, maxLambda, Side::Front, 1 );

  correctResults.clear();
  
  correctResults.push_back( 0.12980836515479821 );
  correctResults.push_back( 0.13146561286817265 );
  correctResults.push_back( 0.12262081451815939 );
  correctResults.push_back( 0.11895717908512865 );
  correctResults.push_back( 0.12262081451815939 );
  correctResults.push_back( 0.13146561286817265 );
  correctResults.push_back( 0.13652493397287235 );
  correctResults.push_back( 0.13805807199254119 );
  correctResults.push_back( 0.13652493397287235 );
  correctResults.push_back( 0.13609579569708838 );
  correctResults.push_back( 0.1223200648357496 );
  correctResults.push_back( 0.11223552993385501 );
  correctResults.push_back( 0.10854433397441074 );
  correctResults.push_back( 0.11223552993385501 );
  correctResults.push_back( 0.1223200648357496 );
  correctResults.push_back( 0.13609579569708838 );
  correctResults.push_back( 0.14296523231421812 );
  correctResults.push_back( 0.14553210014572049 );
  correctResults.push_back( 0.14647163898020743 );
  correctResults.push_back( 0.14553210014572049 );
  correctResults.push_back( 0.14296523231421812 );
  correctResults.push_back( 0.14152475157330446 );
  correctResults.push_back( 0.11684030779255011 );
  correctResults.push_back( 0.10867430462305433 );
  correctResults.push_back( 0.11157326514471867 );
  correctResults.push_back( 0.10867430462305433 );
  correctResults.push_back( 0.11684030779255011 );
  correctResults.push_back( 0.14152475157330446 );
  correctResults.push_back( 0.15098819752429751 );
  correctResults.push_back( 0.15343513493049754 );
  correctResults.push_back( 0.1541829979494049 );
  correctResults.push_back( 0.15343513493049754 );
  correctResults.push_back( 0.15098819752429751 );
  correctResults.push_back( 0.130791990389061 );
  correctResults.push_back( 0.12250965239060073 );
  correctResults.push_back( 0.12449306652651045 );
  correctResults.push_back( 0.12250965239060073 );
  correctResults.push_back( 0.130791990389061 );
  correctResults.push_back( 0.1402590860491354 );
  correctResults.push_back( 0.14105537889949377 );
  correctResults.push_back( 0.1402590860491354 );
  
  EXPECT_EQ( correctResults.size(), aAbsF.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aAbsF[ i ], 1e-6 );
  }

  // Front absorptance layer 2
  aAbsF = *aLayer.Abs( minLambda, maxLambda, Side::Front, 2 );

  correctResults.clear();
  
  correctResults.push_back( 0.25632961379131874 );
  correctResults.push_back( 0.25573547291740378 );
  correctResults.push_back( 0.19697950112706986 );
  correctResults.push_back( 0.17264198074110249 );
  correctResults.push_back( 0.19697950112706986 );
  correctResults.push_back( 0.25573547291740384 );
  correctResults.push_back( 0.26649422802032147 );
  correctResults.push_back( 0.26381852408227446 );
  correctResults.push_back( 0.26649422802032147 );
  correctResults.push_back( 0.25275969849909036 );
  correctResults.push_back( 0.1609393633363895 );
  correctResults.push_back( 0.093722212829289275 );
  correctResults.push_back( 0.069119028173688188 );
  correctResults.push_back( 0.093722212829289275 );
  correctResults.push_back( 0.1609393633363895 );
  correctResults.push_back( 0.25275969849909041 );
  correctResults.push_back( 0.25991370771399275 );
  correctResults.push_back( 0.25408005432175662 );
  correctResults.push_back( 0.25194478898332512 );
  correctResults.push_back( 0.25408005432175662 );
  correctResults.push_back( 0.25991370771399264 );
  correctResults.push_back( 0.24040654653876581 );
  correctResults.push_back( 0.074960934429331647 );
  correctResults.push_back( 0.044765408719739282 );
  correctResults.push_back( 0.087762999098867311 );
  correctResults.push_back( 0.044765408719739198 );
  correctResults.push_back( 0.074960934429331688 );
  correctResults.push_back( 0.24040654653876575 );
  correctResults.push_back( 0.24046876322639604 );
  correctResults.push_back( 0.23441274013102836 );
  correctResults.push_back( 0.232500189733236 );
  correctResults.push_back( 0.23441274013102836 );
  correctResults.push_back( 0.24046876322639604 );
  correctResults.push_back( 0.15266829159485601 );
  correctResults.push_back( 0.17248602062174306 );
  correctResults.push_back( 0.17853560592413201 );
  correctResults.push_back( 0.17248602062174304 );
  correctResults.push_back( 0.15266829159485601 );
  correctResults.push_back( 0.14367286475673283 );
  correctResults.push_back( 0.14153356122146118 );
  correctResults.push_back( 0.1436728647567328 );
  
  EXPECT_EQ( correctResults.size(), aAbsF.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aAbsF[ i ], 1e-6 );
  }

  // Back absorptance layer 1
  vector< double > aAbsB = *aLayer.Abs( minLambda, maxLambda, Side::Back, 1 );
  
  correctResults.clear();
  
  correctResults.push_back( 0.026637501841971487 );
  correctResults.push_back( 0.026740074434937242 );
  correctResults.push_back( 0.0199791403464591 );
  correctResults.push_back( 0.018548540557078604 );
  correctResults.push_back( 0.0199791403464591 );
  correctResults.push_back( 0.026740074434937242 );
  correctResults.push_back( 0.042843751780201847 );
  correctResults.push_back( 0.049553236610345712 );
  correctResults.push_back( 0.042843751780201847 );
  correctResults.push_back( 0.027049263783772724 );
  correctResults.push_back( 0.01822289247295962 );
  correctResults.push_back( 0.016006887520252974 );
  correctResults.push_back( 0.015195773412650201 );
  correctResults.push_back( 0.016006887520252974 );
  correctResults.push_back( 0.01822289247295962 );
  correctResults.push_back( 0.027049263783772724 );
  correctResults.push_back( 0.054431052096806286 );
  correctResults.push_back( 0.074608745773621241 );
  correctResults.push_back( 0.082025225283261699 );
  correctResults.push_back( 0.074608745773621241 );
  correctResults.push_back( 0.054431052096806286 );
  correctResults.push_back( 0.027564281684927104 );
  correctResults.push_back( 0.015515354875024756 );
  correctResults.push_back( 0.013364352231569933 );
  correctResults.push_back( 0.012717582786376653 );
  correctResults.push_back( 0.013364352231569933 );
  correctResults.push_back( 0.015515354875024756 );
  correctResults.push_back( 0.027564281684927104 );
  correctResults.push_back( 0.085743957809190752 );
  correctResults.push_back( 0.10146807913335643 );
  correctResults.push_back( 0.091016404567913198 );
  correctResults.push_back( 0.10146807913335644 );
  correctResults.push_back( 0.085743957809190752 );
  correctResults.push_back( 0.028482449268704017 );
  correctResults.push_back( 0.010605184137024846 );
  correctResults.push_back( 0.0095235845928839432 );
  correctResults.push_back( 0.010605184137024846 );
  correctResults.push_back( 0.028482449268704017 );
  correctResults.push_back( 0.040171972090904437 );
  correctResults.push_back( 0.034457448020082902 );
  correctResults.push_back( 0.040171972090904437 );
  
  EXPECT_EQ( correctResults.size(), aAbsB.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aAbsB[ i ], 1e-6 );
  }

  // Back absorptance layer 2
  aAbsB = *aLayer.Abs( minLambda, maxLambda, Side::Back, 2 );
  
  correctResults.clear();
  
  correctResults.push_back( 0.29974496491378699 );
  correctResults.push_back( 0.29974723688163807 );
  correctResults.push_back( 0.30837758590183517 );
  correctResults.push_back( 0.3037895266641607 );
  correctResults.push_back( 0.30837758590183517 );
  correctResults.push_back( 0.29974723688163807 );
  correctResults.push_back( 0.23645017323399389 );
  correctResults.push_back( 0.2101216541209924 );
  correctResults.push_back( 0.23645017323399389 );
  correctResults.push_back( 0.29982528842648154 );
  correctResults.push_back( 0.30242049565857915 );
  correctResults.push_back( 0.29300799242868925 );
  correctResults.push_back( 0.28956277713334655 );
  correctResults.push_back( 0.29300799242868925 );
  correctResults.push_back( 0.30242049565857915 );
  correctResults.push_back( 0.29982528842648154 );
  correctResults.push_back( 0.20006827714467729 );
  correctResults.push_back( 0.12664787384502862 );
  correctResults.push_back( 0.09970399364783053 );
  correctResults.push_back( 0.12664787384502851 );
  correctResults.push_back( 0.20006827714467734 );
  correctResults.push_back( 0.30039972030121548 );
  correctResults.push_back( 0.29092020259538748 );
  correctResults.push_back( 0.28081260174986611 );
  correctResults.push_back( 0.27764327694146923 );
  correctResults.push_back( 0.28081260174986611 );
  correctResults.push_back( 0.29092020259538748 );
  correctResults.push_back( 0.30039972030121548 );
  correctResults.push_back( 0.11741511404490962 );
  correctResults.push_back( 0.08664134309367641 );
  correctResults.push_back( 0.13737264526651424 );
  correctResults.push_back( 0.08664134309367634 );
  correctResults.push_back( 0.11741511404490962 );
  correctResults.push_back( 0.30537329036447852 );
  correctResults.push_back( 0.26699226478906757 );
  correctResults.push_back( 0.26140948534495562 );
  correctResults.push_back( 0.26699226478906757 );
  correctResults.push_back( 0.30537329036447852 );
  correctResults.push_back( 0.35060218345195399 );
  correctResults.push_back( 0.35928301311296368 );
  correctResults.push_back( 0.35060218345195399 );
  
  EXPECT_EQ( correctResults.size(), aAbsB.size() );
  for( size_t i = 0; i < size; ++i ) {
    EXPECT_NEAR( correctResults[ i ], aAbsB[ i ], 1e-6 );
  }

}