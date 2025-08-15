#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SingleLayerOptics;
using namespace FenestrationCommon;
using namespace SpectralAveraging;
using namespace MultiLayerOptics;

// Example/ test case on how to create double layer with venetian blind on interior side (venetian
// blind is second layer). Venetian blind in this case is made of material that is measured at
// different wavelengths.

class MultiPaneBSDF_102_VenetianUniformMultiWL : public testing::Test
{
private:
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

    std::shared_ptr<CSpectralSampleData> loadSampleData_NFRC_102()
    {
        auto aMeasurements_102 = CSpectralSampleData::create(
          {{0.300, 0.0020, 0.0020, 0.0470, 0.0480}, {0.305, 0.0030, 0.0030, 0.0470, 0.0480},
           {0.310, 0.0090, 0.0090, 0.0470, 0.0480}, {0.315, 0.0350, 0.0350, 0.0470, 0.0480},
           {0.320, 0.1000, 0.1000, 0.0470, 0.0480}, {0.325, 0.2180, 0.2180, 0.0490, 0.0500},
           {0.330, 0.3560, 0.3560, 0.0530, 0.0540}, {0.335, 0.4980, 0.4980, 0.0600, 0.0610},
           {0.340, 0.6160, 0.6160, 0.0670, 0.0670}, {0.345, 0.7090, 0.7090, 0.0730, 0.0740},
           {0.350, 0.7740, 0.7740, 0.0780, 0.0790}, {0.355, 0.8180, 0.8180, 0.0820, 0.0820},
           {0.360, 0.8470, 0.8470, 0.0840, 0.0840}, {0.365, 0.8630, 0.8630, 0.0850, 0.0850},
           {0.370, 0.8690, 0.8690, 0.0850, 0.0860}, {0.375, 0.8610, 0.8610, 0.0850, 0.0850},
           {0.380, 0.8560, 0.8560, 0.0840, 0.0840}, {0.385, 0.8660, 0.8660, 0.0850, 0.0850},
           {0.390, 0.8810, 0.8810, 0.0860, 0.0860}, {0.395, 0.8890, 0.8890, 0.0860, 0.0860},
           {0.400, 0.8930, 0.8930, 0.0860, 0.0860}, {0.410, 0.8930, 0.8930, 0.0860, 0.0860},
           {0.420, 0.8920, 0.8920, 0.0860, 0.0860}, {0.430, 0.8920, 0.8920, 0.0850, 0.0850},
           {0.440, 0.8920, 0.8920, 0.0850, 0.0850}, {0.450, 0.8960, 0.8960, 0.0850, 0.0850},
           {0.460, 0.9000, 0.9000, 0.0850, 0.0850}, {0.470, 0.9020, 0.9020, 0.0840, 0.0840},
           {0.480, 0.9030, 0.9030, 0.0840, 0.0840}, {0.490, 0.9040, 0.9040, 0.0850, 0.0850},
           {0.500, 0.9050, 0.9050, 0.0840, 0.0840}, {0.510, 0.9050, 0.9050, 0.0840, 0.0840},
           {0.520, 0.9050, 0.9050, 0.0840, 0.0840}, {0.530, 0.9040, 0.9040, 0.0840, 0.0840},
           {0.540, 0.9040, 0.9040, 0.0830, 0.0830}, {0.550, 0.9030, 0.9030, 0.0830, 0.0830},
           {0.560, 0.9020, 0.9020, 0.0830, 0.0830}, {0.570, 0.9000, 0.9000, 0.0820, 0.0820},
           {0.580, 0.8980, 0.8980, 0.0820, 0.0820}, {0.590, 0.8960, 0.8960, 0.0810, 0.0810},
           {0.600, 0.8930, 0.8930, 0.0810, 0.0810}, {0.610, 0.8900, 0.8900, 0.0810, 0.0810},
           {0.620, 0.8860, 0.8860, 0.0800, 0.0800}, {0.630, 0.8830, 0.8830, 0.0800, 0.0800},
           {0.640, 0.8790, 0.8790, 0.0790, 0.0790}, {0.650, 0.8750, 0.8750, 0.0790, 0.0790},
           {0.660, 0.8720, 0.8720, 0.0790, 0.0790}, {0.670, 0.8680, 0.8680, 0.0780, 0.0780},
           {0.680, 0.8630, 0.8630, 0.0780, 0.0780}, {0.690, 0.8590, 0.8590, 0.0770, 0.0770},
           {0.700, 0.8540, 0.8540, 0.0760, 0.0770}, {0.710, 0.8500, 0.8500, 0.0760, 0.0760},
           {0.720, 0.8450, 0.8450, 0.0750, 0.0760}, {0.730, 0.8400, 0.8400, 0.0750, 0.0750},
           {0.740, 0.8350, 0.8350, 0.0750, 0.0750}, {0.750, 0.8310, 0.8310, 0.0740, 0.0740},
           {0.760, 0.8260, 0.8260, 0.0740, 0.0740}, {0.770, 0.8210, 0.8210, 0.0740, 0.0740},
           {0.780, 0.8160, 0.8160, 0.0730, 0.0730}, {0.790, 0.8120, 0.8120, 0.0730, 0.0730},
           {0.800, 0.8080, 0.8080, 0.0720, 0.0720}, {0.810, 0.8030, 0.8030, 0.0720, 0.0720},
           {0.820, 0.8000, 0.8000, 0.0720, 0.0720}, {0.830, 0.7960, 0.7960, 0.0710, 0.0710},
           {0.840, 0.7930, 0.7930, 0.0700, 0.0710}, {0.850, 0.7880, 0.7880, 0.0700, 0.0710},
           {0.860, 0.7860, 0.7860, 0.0700, 0.0700}, {0.870, 0.7820, 0.7820, 0.0740, 0.0740},
           {0.880, 0.7800, 0.7800, 0.0720, 0.0720}, {0.890, 0.7770, 0.7770, 0.0730, 0.0740},
           {0.900, 0.7760, 0.7760, 0.0720, 0.0720}, {0.910, 0.7730, 0.7730, 0.0720, 0.0720},
           {0.920, 0.7710, 0.7710, 0.0710, 0.0710}, {0.930, 0.7700, 0.7700, 0.0700, 0.0700},
           {0.940, 0.7680, 0.7680, 0.0690, 0.0690}, {0.950, 0.7660, 0.7660, 0.0680, 0.0680},
           {0.960, 0.7660, 0.7660, 0.0670, 0.0680}, {0.970, 0.7640, 0.7640, 0.0680, 0.0680},
           {0.980, 0.7630, 0.7630, 0.0680, 0.0680}, {0.990, 0.7620, 0.7620, 0.0670, 0.0670},
           {1.000, 0.7620, 0.7620, 0.0660, 0.0670}, {1.050, 0.7600, 0.7600, 0.0660, 0.0660},
           {1.100, 0.7590, 0.7590, 0.0660, 0.0660}, {1.150, 0.7610, 0.7610, 0.0660, 0.0660},
           {1.200, 0.7650, 0.7650, 0.0660, 0.0660}, {1.250, 0.7700, 0.7700, 0.0650, 0.0650},
           {1.300, 0.7770, 0.7770, 0.0670, 0.0670}, {1.350, 0.7860, 0.7860, 0.0660, 0.0670},
           {1.400, 0.7950, 0.7950, 0.0670, 0.0680}, {1.450, 0.8080, 0.8080, 0.0670, 0.0670},
           {1.500, 0.8190, 0.8190, 0.0690, 0.0690}, {1.550, 0.8290, 0.8290, 0.0690, 0.0690},
           {1.600, 0.8360, 0.8360, 0.0700, 0.0700}, {1.650, 0.8400, 0.8400, 0.0700, 0.0700},
           {1.700, 0.8420, 0.8420, 0.0690, 0.0700}, {1.750, 0.8420, 0.8420, 0.0690, 0.0700},
           {1.800, 0.8410, 0.8410, 0.0700, 0.0700}, {1.850, 0.8400, 0.8400, 0.0690, 0.0690},
           {1.900, 0.8390, 0.8390, 0.0680, 0.0680}, {1.950, 0.8390, 0.8390, 0.0710, 0.0710},
           {2.000, 0.8390, 0.8390, 0.0690, 0.0690}, {2.050, 0.8400, 0.8400, 0.0680, 0.0680},
           {2.100, 0.8410, 0.8410, 0.0680, 0.0680}, {2.150, 0.8390, 0.8390, 0.0690, 0.0690},
           {2.200, 0.8300, 0.8300, 0.0700, 0.0700}, {2.250, 0.8300, 0.8300, 0.0700, 0.0700},
           {2.300, 0.8320, 0.8320, 0.0690, 0.0690}, {2.350, 0.8320, 0.8320, 0.0690, 0.0700},
           {2.400, 0.8320, 0.8320, 0.0700, 0.0700}, {2.450, 0.8260, 0.8260, 0.0690, 0.0690},
           {2.500, 0.8220, 0.8220, 0.0680, 0.0680}});

        return aMeasurements_102;
    }

    std::shared_ptr<CSpectralSampleData> loadVenetianBlindMaterial()
    {
        std::shared_ptr<CSpectralSampleData> aMeasurements_Venetian =
          std::make_shared<CSpectralSampleData>();

        aMeasurements_Venetian->addRecord(0.30, 0, 0.08, 0.08);
        aMeasurements_Venetian->addRecord(0.32, 0, 0.08, 0.08);
        aMeasurements_Venetian->addRecord(0.34, 0, 0.07, 0.07);
        aMeasurements_Venetian->addRecord(0.36, 0, 0.08, 0.08);
        aMeasurements_Venetian->addRecord(0.38, 0, 0.14, 0.14);
        aMeasurements_Venetian->addRecord(0.40, 0, 0.37, 0.37);
        aMeasurements_Venetian->addRecord(0.42, 0, 0.47, 0.47);
        aMeasurements_Venetian->addRecord(0.44, 0, 0.47, 0.47);
        aMeasurements_Venetian->addRecord(0.46, 0, 0.47, 0.47);
        aMeasurements_Venetian->addRecord(0.48, 0, 0.50, 0.50);
        aMeasurements_Venetian->addRecord(0.50, 0, 0.49, 0.49);
        aMeasurements_Venetian->addRecord(0.52, 0, 0.47, 0.47);
        aMeasurements_Venetian->addRecord(0.54, 0, 0.47, 0.47);
        aMeasurements_Venetian->addRecord(0.56, 0, 0.45, 0.45);
        aMeasurements_Venetian->addRecord(0.58, 0, 0.49, 0.49);
        aMeasurements_Venetian->addRecord(0.60, 0, 0.48, 0.48);
        aMeasurements_Venetian->addRecord(0.62, 0, 0.44, 0.44);
        aMeasurements_Venetian->addRecord(0.64, 0, 0.42, 0.42);
        aMeasurements_Venetian->addRecord(0.66, 0, 0.42, 0.42);
        aMeasurements_Venetian->addRecord(0.68, 0, 0.44, 0.44);
        aMeasurements_Venetian->addRecord(0.70, 0, 0.47, 0.47);
        aMeasurements_Venetian->addRecord(0.72, 0, 0.46, 0.46);
        aMeasurements_Venetian->addRecord(0.74, 0, 0.46, 0.46);
        aMeasurements_Venetian->addRecord(0.76, 0, 0.49, 0.49);
        aMeasurements_Venetian->addRecord(0.78, 0, 0.49, 0.49);
        aMeasurements_Venetian->addRecord(0.80, 0, 0.54, 0.54);
        aMeasurements_Venetian->addRecord(0.85, 0, 0.62, 0.62);
        aMeasurements_Venetian->addRecord(0.90, 0, 0.63, 0.63);
        aMeasurements_Venetian->addRecord(0.95, 0, 0.64, 0.64);
        aMeasurements_Venetian->addRecord(1.00, 0, 0.64, 0.64);
        aMeasurements_Venetian->addRecord(1.05, 0, 0.65, 0.65);
        aMeasurements_Venetian->addRecord(1.10, 0, 0.65, 0.65);
        aMeasurements_Venetian->addRecord(1.15, 0, 0.65, 0.65);
        aMeasurements_Venetian->addRecord(1.20, 0, 0.66, 0.66);
        aMeasurements_Venetian->addRecord(1.25, 0, 0.66, 0.66);
        aMeasurements_Venetian->addRecord(1.30, 0, 0.66, 0.66);
        aMeasurements_Venetian->addRecord(1.35, 0, 0.66, 0.66);
        aMeasurements_Venetian->addRecord(1.40, 0, 0.67, 0.67);
        aMeasurements_Venetian->addRecord(1.45, 0, 0.67, 0.67);
        aMeasurements_Venetian->addRecord(1.50, 0, 0.68, 0.68);
        aMeasurements_Venetian->addRecord(1.55, 0, 0.69, 0.69);
        aMeasurements_Venetian->addRecord(1.60, 0, 0.70, 0.70);
        aMeasurements_Venetian->addRecord(1.65, 0, 0.70, 0.70);
        aMeasurements_Venetian->addRecord(1.70, 0, 0.69, 0.69);
        aMeasurements_Venetian->addRecord(1.75, 0, 0.70, 0.70);
        aMeasurements_Venetian->addRecord(1.80, 0, 0.71, 0.71);
        aMeasurements_Venetian->addRecord(1.85, 0, 0.72, 0.72);
        aMeasurements_Venetian->addRecord(1.90, 0, 0.72, 0.72);
        aMeasurements_Venetian->addRecord(1.95, 0, 0.72, 0.72);
        aMeasurements_Venetian->addRecord(2.00, 0, 0.73, 0.73);
        aMeasurements_Venetian->addRecord(2.05, 0, 0.74, 0.74);
        aMeasurements_Venetian->addRecord(2.10, 0, 0.74, 0.74);
        aMeasurements_Venetian->addRecord(2.20, 0, 0.74, 0.74);
        aMeasurements_Venetian->addRecord(2.30, 0, 0.69, 0.69);
        aMeasurements_Venetian->addRecord(2.40, 0, 0.73, 0.73);
        aMeasurements_Venetian->addRecord(2.50, 0, 0.74, 0.74);

        return aMeasurements_Venetian;
    }

protected:
    virtual void SetUp()
    {
        constexpr auto GlassThickness = 3.048e-3;   // [m]
        auto aMaterial_102 = Material::nBandMaterial(
          SpectralSample::NFRC_102(), GlassThickness, MaterialType::Monolithic);

        const auto aBSDF = BSDFHemisphere::create(BSDFBasis::Small);

        // specular layer NFRC=102
        auto Layer_102 = CBSDFLayerMaker::getSpecularLayer(aMaterial_102, aBSDF);


        constexpr auto ShadeThickness = 1.5e-3;   // [m]
        auto aMaterial_Venetian = Material::nBandMaterial(
          loadVenetianBlindMaterial(), ShadeThickness, MaterialType::Monolithic);

        // make cell geometry
        constexpr auto slatWidth = 0.016;     // m
        constexpr auto slatSpacing = 0.012;   // m
        constexpr auto slatTiltAngle = 0.0;
        constexpr auto curvatureRadius = 0.0;
        constexpr size_t numOfSlatSegments = 5;

        // get shading BSDF layer
        auto aVenetian = CBSDFLayerMaker::getVenetianLayer(aMaterial_Venetian,
                                                           aBSDF,
                                                           slatWidth,
                                                           slatSpacing,
                                                           slatTiltAngle,
                                                           curvatureRadius,
                                                           numOfSlatSegments,
                                                           DistributionMethod::UniformDiffuse,
                                                           true);

        auto commonWavelengths = aVenetian->getBandWavelengths();

        m_Layer = CMultiPaneBSDF::create({Layer_102, aVenetian}, commonWavelengths);

        const CalculationProperties input{StandardData::solarRadiationASTM_E891_87_Table1(),
                                          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer()
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_102_VenetianUniformMultiWL, TestBSDF1)
{
    SCOPED_TRACE("Begin Test: Specular and venetian uniform IGU with multiwavlength "
                 "material data applied to shading device - BSDF.");

    const double minLambda = 0.3;
    const double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    SquareMatrix aT = aLayer.getMatrix(minLambda, maxLambda, Side::Front, PropertySimple::T);

    // Front transmittance matrix
    size_t size = aT.size();

    std::vector<double> correctResults{20.704876361082952,
                                       2.6876261056026207,
                                       1.4884587821243103,
                                       1.184224847758828,
                                       1.1516440264754242,
                                       1.3007908721969141,
                                       1.1473327594840519};


    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Back Reflectance matrix
    SquareMatrix aRb = aLayer.getMatrix(minLambda, maxLambda, Side::Back, PropertySimple::R);

    correctResults = {1.8592983854164531,
                      0.24163057657322592,
                      0.13608742192145262,
                      0.11809363568173475,
                      0.15319382630343492,
                      0.34329561288490762,
                      1.6850908192028509};

    EXPECT_EQ(correctResults.size(), aRb.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRb(i, i), 1e-6);
    }

    // Front absorptance layer 1
    auto aAbsF = aLayer.Abs(minLambda, maxLambda, Side::Front, 1);

    correctResults = {0.091741474208008794,
                      0.092681337861886426,
                      0.095467541364117794,
                      0.099922354300501642,
                      0.10542702702814911,
                      0.11012252597234512,
                      0.10425307616990795};

    EXPECT_EQ(correctResults.size(), aAbsF.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aAbsF[i], 1e-6);
    }

    // Front absorptance layer 2
    aAbsF = aLayer.Abs(minLambda, maxLambda, Side::Front, 2);

    correctResults = {0, 0, 0, 0, 0, 0, 0};

    EXPECT_EQ(correctResults.size(), aAbsF.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aAbsF[i], 1e-6);
    }

    // Back absorptance layer 1
    auto aAbsB = aLayer.Abs(minLambda, maxLambda, Side::Back, 1);

    correctResults = {0.091579680636289171,
                      0.092519510646167868,
                      0.095305151046115769,
                      0.099757516080865291,
                      0.10525646069344624,
                      0.10994564072275897,
                      0.10409705446127393};

    EXPECT_EQ(correctResults.size(), aAbsB.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aAbsB[i], 1e-6);
    }

    // Back absorptance layer 2
    aAbsB = aLayer.Abs(minLambda, maxLambda, Side::Back, 2);

    correctResults = {0, 0, 0, 0, 0, 0, 0};

    EXPECT_EQ(correctResults.size(), aAbsB.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aAbsB[i], 1e-6);
    }
}
