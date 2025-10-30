#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCEMultiLayerOptics.hpp>
#include <WCESingleLayerOptics.hpp>
#include <WCECommon.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

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
          {0.30, {0, 0, 0.08, 0.08}, {0.0, 0.0, 0.0, 0.0}},
          {0.32, {0, 0, 0.08, 0.08}, {0.0, 0.0, 0.0, 0.0}},
          {0.34, {0, 0, 0.07, 0.07}, {0.0, 0.0, 0.0, 0.0}},
          {0.36, {0, 0, 0.08, 0.08}, {0.0, 0.0, 0.0, 0.0}},
          {0.38, {0, 0, 0.14, 0.14}, {0.0, 0.0, 0.0, 0.0}},
          {0.40, {0, 0, 0.37, 0.37}, {0.0, 0.0, 0.0, 0.0}},
          {0.42, {0, 0, 0.47, 0.47}, {0.0, 0.0, 0.0, 0.0}},
          {0.44, {0, 0, 0.47, 0.47}, {0.0, 0.0, 0.0, 0.0}},
          {0.46, {0, 0, 0.47, 0.47}, {0.0, 0.0, 0.0, 0.0}},
          {0.48, {0, 0, 0.50, 0.50}, {0.0, 0.0, 0.0, 0.0}},
          {0.50, {0, 0, 0.49, 0.49}, {0.0, 0.0, 0.0, 0.0}},
          {0.52, {0, 0, 0.47, 0.47}, {0.0, 0.0, 0.0, 0.0}},
          {0.54, {0, 0, 0.47, 0.47}, {0.0, 0.0, 0.0, 0.0}},
          {0.56, {0, 0, 0.45, 0.45}, {0.0, 0.0, 0.0, 0.0}},
          {0.58, {0, 0, 0.49, 0.49}, {0.0, 0.0, 0.0, 0.0}},
          {0.60, {0, 0, 0.48, 0.48}, {0.0, 0.0, 0.0, 0.0}},
          {0.62, {0, 0, 0.44, 0.44}, {0.0, 0.0, 0.0, 0.0}},
          {0.64, {0, 0, 0.42, 0.42}, {0.0, 0.0, 0.0, 0.0}},
          {0.66, {0, 0, 0.42, 0.42}, {0.0, 0.0, 0.0, 0.0}},
          {0.68, {0, 0, 0.44, 0.44}, {0.0, 0.0, 0.0, 0.0}},
          {0.70, {0, 0, 0.47, 0.47}, {0.0, 0.0, 0.0, 0.0}},
          {0.72, {0, 0, 0.46, 0.46}, {0.0, 0.0, 0.0, 0.0}},
          {0.74, {0, 0, 0.46, 0.46}, {0.0, 0.0, 0.0, 0.0}},
          {0.76, {0, 0, 0.49, 0.49}, {0.0, 0.0, 0.0, 0.0}},
          {0.78, {0, 0, 0.49, 0.49}, {0.0, 0.0, 0.0, 0.0}},
          {0.80, {0, 0, 0.54, 0.54}, {0.0, 0.0, 0.0, 0.0}},
          {0.85, {0, 0, 0.62, 0.62}, {0.0, 0.0, 0.0, 0.0}},
          {0.90, {0, 0, 0.63, 0.63}, {0.0, 0.0, 0.0, 0.0}},
          {0.95, {0, 0, 0.64, 0.64}, {0.0, 0.0, 0.0, 0.0}},
          {1.00, {0, 0, 0.64, 0.64}, {0.0, 0.0, 0.0, 0.0}},
          {1.05, {0, 0, 0.65, 0.65}, {0.0, 0.0, 0.0, 0.0}},
          {1.10, {0, 0, 0.65, 0.65}, {0.0, 0.0, 0.0, 0.0}},
          {1.15, {0, 0, 0.65, 0.65}, {0.0, 0.0, 0.0, 0.0}},
          {1.20, {0, 0, 0.66, 0.66}, {0.0, 0.0, 0.0, 0.0}},
          {1.25, {0, 0, 0.66, 0.66}, {0.0, 0.0, 0.0, 0.0}},
          {1.30, {0, 0, 0.66, 0.66}, {0.0, 0.0, 0.0, 0.0}},
          {1.35, {0, 0, 0.66, 0.66}, {0.0, 0.0, 0.0, 0.0}},
          {1.40, {0, 0, 0.67, 0.67}, {0.0, 0.0, 0.0, 0.0}},
          {1.45, {0, 0, 0.67, 0.67}, {0.0, 0.0, 0.0, 0.0}},
          {1.50, {0, 0, 0.68, 0.68}, {0.0, 0.0, 0.0, 0.0}},
          {1.55, {0, 0, 0.69, 0.69}, {0.0, 0.0, 0.0, 0.0}},
          {1.60, {0, 0, 0.70, 0.70}, {0.0, 0.0, 0.0, 0.0}},
          {1.65, {0, 0, 0.70, 0.70}, {0.0, 0.0, 0.0, 0.0}},
          {1.70, {0, 0, 0.69, 0.69}, {0.0, 0.0, 0.0, 0.0}},
          {1.75, {0, 0, 0.70, 0.70}, {0.0, 0.0, 0.0, 0.0}},
          {1.80, {0, 0, 0.71, 0.71}, {0.0, 0.0, 0.0, 0.0}},
          {1.85, {0, 0, 0.72, 0.72}, {0.0, 0.0, 0.0, 0.0}},
          {1.90, {0, 0, 0.72, 0.72}, {0.0, 0.0, 0.0, 0.0}},
          {1.95, {0, 0, 0.72, 0.72}, {0.0, 0.0, 0.0, 0.0}},
          {2.00, {0, 0, 0.73, 0.73}, {0.0, 0.0, 0.0, 0.0}},
          {2.05, {0, 0, 0.74, 0.74}, {0.0, 0.0, 0.0, 0.0}},
          {2.10, {0, 0, 0.74, 0.74}, {0.0, 0.0, 0.0, 0.0}},
          {2.20, {0, 0, 0.74, 0.74}, {0.0, 0.0, 0.0, 0.0}},
          {2.30, {0, 0, 0.69, 0.69}, {0.0, 0.0, 0.0, 0.0}},
          {2.40, {0, 0, 0.73, 0.73}, {0.0, 0.0, 0.0, 0.0}},
          {2.50, {0, 0, 0.74, 0.74}, {0.0, 0.0, 0.0, 0.0}},
        });
    }

protected:
    void SetUp() override
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
    }
};

TEST_F(MultiPaneBSDF_102_VenetianUniformMultiWL, TestBSDF1)
{
    SCOPED_TRACE("Begin Test: Specular and venetian uniform IGU with multiwavelength "
                 "material data applied to shading device - BSDF.");

    constexpr double minLambda = 0.3;
    constexpr double maxLambda = 2.5;

    CMultiPaneBSDF & aLayer = getLayer();

    SquareMatrix aT = aLayer.getMatrix(minLambda, maxLambda, Side::Front, PropertySurface::T);

    // Front transmittance matrix
    size_t size = aT.size();

    std::vector correctResults{
      20.704876, 2.687626, 1.488459, 1.184225, 1.151644, 1.300791, 1.147333};

    EXPECT_EQ(correctResults.size(), aT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aT(i, i), 1e-6);
    }

    // Back Reflectance matrix
    SquareMatrix aRb = aLayer.getMatrix(minLambda, maxLambda, Side::Back, PropertySurface::R);

    correctResults = {1.859298, 0.241631, 0.136087, 0.118094, 0.153194, 0.343296, 1.685091};

    EXPECT_EQ(correctResults.size(), aRb.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aRb(i, i), 1e-6);
    }

    // Front absorptance layer 1
    auto aAbsF = aLayer.Abs(minLambda, maxLambda, Side::Front, 1);

    correctResults = {0.091741, 0.092681, 0.095468, 0.099922, 0.105427, 0.110123, 0.104253};

    EXPECT_EQ(correctResults.size(), aAbsF.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aAbsF[i], 1e-6);
    }

    // Front absorptance layer 2
    aAbsF = aLayer.Abs(minLambda, maxLambda, Side::Front, 2);

    correctResults = {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000};

    EXPECT_EQ(correctResults.size(), aAbsF.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aAbsF[i], 1e-6);
    }

    // Back absorptance layer 1
    auto aAbsB = aLayer.Abs(minLambda, maxLambda, Side::Back, 1);

    correctResults = {0.091580, 0.092517, 0.095294, 0.099735, 0.105219, 0.109895, 0.104039};

    EXPECT_EQ(correctResults.size(), aAbsB.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aAbsB[i], 1e-6);
    }

    // Back absorptance layer 2
    aAbsB = aLayer.Abs(minLambda, maxLambda, Side::Back, 2);

    correctResults = {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000};

    EXPECT_EQ(correctResults.size(), aAbsB.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctResults[i], aAbsB[i], 1e-6);
    }
}
