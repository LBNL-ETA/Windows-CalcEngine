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
    std::unique_ptr<CMultiPaneBSDF> m_Layer;

    static std::shared_ptr<CSpectralSampleData> loadVenetianBlindMaterial()
    {
        return CSpectralSampleData::create({
          {0.30, 0, 0, 0.08, 0.08}, {0.32, 0, 0, 0.08, 0.08}, {0.34, 0, 0, 0.07, 0.07},
          {0.36, 0, 0, 0.08, 0.08}, {0.38, 0, 0, 0.14, 0.14}, {0.40, 0, 0, 0.37, 0.37},
          {0.42, 0, 0, 0.47, 0.47}, {0.44, 0, 0, 0.47, 0.47}, {0.46, 0, 0, 0.47, 0.47},
          {0.48, 0, 0, 0.50, 0.50}, {0.50, 0, 0, 0.49, 0.49}, {0.52, 0, 0, 0.47, 0.47},
          {0.54, 0, 0, 0.47, 0.47}, {0.56, 0, 0, 0.45, 0.45}, {0.58, 0, 0, 0.49, 0.49},
          {0.60, 0, 0, 0.48, 0.48}, {0.62, 0, 0, 0.44, 0.44}, {0.64, 0, 0, 0.42, 0.42},
          {0.66, 0, 0, 0.42, 0.42}, {0.68, 0, 0, 0.44, 0.44}, {0.70, 0, 0, 0.47, 0.47},
          {0.72, 0, 0, 0.46, 0.46}, {0.74, 0, 0, 0.46, 0.46}, {0.76, 0, 0, 0.49, 0.49},
          {0.78, 0, 0, 0.49, 0.49}, {0.80, 0, 0, 0.54, 0.54}, {0.85, 0, 0, 0.62, 0.62},
          {0.90, 0, 0, 0.63, 0.63}, {0.95, 0, 0, 0.64, 0.64}, {1.00, 0, 0, 0.64, 0.64},
          {1.05, 0, 0, 0.65, 0.65}, {1.10, 0, 0, 0.65, 0.65}, {1.15, 0, 0, 0.65, 0.65},
          {1.20, 0, 0, 0.66, 0.66}, {1.25, 0, 0, 0.66, 0.66}, {1.30, 0, 0, 0.66, 0.66},
          {1.35, 0, 0, 0.66, 0.66}, {1.40, 0, 0, 0.67, 0.67}, {1.45, 0, 0, 0.67, 0.67},
          {1.50, 0, 0, 0.68, 0.68}, {1.55, 0, 0, 0.69, 0.69}, {1.60, 0, 0, 0.70, 0.70},
          {1.65, 0, 0, 0.70, 0.70}, {1.70, 0, 0, 0.69, 0.69}, {1.75, 0, 0, 0.70, 0.70},
          {1.80, 0, 0, 0.71, 0.71}, {1.85, 0, 0, 0.72, 0.72}, {1.90, 0, 0, 0.72, 0.72},
          {1.95, 0, 0, 0.72, 0.72}, {2.00, 0, 0, 0.73, 0.73}, {2.05, 0, 0, 0.74, 0.74},
          {2.10, 0, 0, 0.74, 0.74}, {2.20, 0, 0, 0.74, 0.74}, {2.30, 0, 0, 0.69, 0.69},
          {2.40, 0, 0, 0.73, 0.73}, {2.50, 0, 0, 0.74, 0.74},
        });
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

        const CalculationProperties input{
          StandardData::solarRadiationASTM_E891_87_Table1(),
          StandardData::solarRadiationASTM_E891_87_Table1().getXArray()};
        m_Layer->setCalculationProperties(input);
    }

public:
    CMultiPaneBSDF & getLayer() const
    {
        return *m_Layer;
    };
};

TEST_F(MultiPaneBSDF_102_VenetianUniformMultiWL, TestBSDF1)
{
    SCOPED_TRACE("Begin Test: Specular and venetian uniform IGU with multiwavelength "
                 "material data applied to shading device - BSDF.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

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
