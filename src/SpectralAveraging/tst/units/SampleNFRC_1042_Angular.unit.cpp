#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "optical/standardData.hpp"
#include "optical/spectralSampleData.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestSampleNFRC_1042_Angular : public testing::Test
{
    CSpectralSample m_Sample{SpectralSample::NFRC_1042(),
                             StandardData::solarRadiationASTM_E891_87_Table1()};

public:
    CSpectralSample & getSample()
    {
        return m_Sample;
    }
};

// C++
TEST_F(TestSampleNFRC_1042_Angular, TestSampleProperties)
{
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    constexpr auto thickness = 3.18e-3;   // [m]

    constexpr auto layerType = MaterialType::Coated;

    constexpr auto angle = 70.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.327258, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.471995, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.504595, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.200746, absorptance, 1e-6);
}
