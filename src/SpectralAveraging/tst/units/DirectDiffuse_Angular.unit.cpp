#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

// Example (test case) of sample that calculates angular properties of single layer sample

class TestDirectDiffuse_Angular : public testing::Test
{
    std::shared_ptr<CAngularSpectralSample> m_Sample;

protected:
    void SetUp() override
    {
        auto aMeasurements = SpectralSample::DirectDiffuse();

        auto aSample = std::make_shared<CSpectralSample>(
          aMeasurements, StandardData::solarRadiationASTM_E891_87_Table1());

        auto thickness = 3e-3;   // [m]
        auto layerType = MaterialType::Monolithic;

        m_Sample = std::make_shared<CAngularSpectralSample>(aSample, thickness, layerType);
    }

public:
    std::shared_ptr<CAngularSpectralSample> getSample() const
    {
        return m_Sample;
    }
};

TEST_F(TestDirectDiffuse_Angular, TestProperties0degrees)
{
    constexpr auto angle{0.0};

    std::shared_ptr<CAngularSpectralSample> angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance =
      angularSample->getProperty(lowLambda, highLambda, Property::T, Side::Front, angle);
    EXPECT_NEAR(0.3, transmittance, 1e-6);

    auto reflectanceFront =
      angularSample->getProperty(lowLambda, highLambda, Property::R, Side::Front, angle);
    EXPECT_NEAR(0.5, reflectanceFront, 1e-6);

    auto reflectanceBack =
      angularSample->getProperty(lowLambda, highLambda, Property::R, Side::Back, angle);
    EXPECT_NEAR(0.5, reflectanceBack, 1e-6);

    auto absorptance =
      angularSample->getProperty(lowLambda, highLambda, Property::Abs, Side::Front, angle);
    EXPECT_NEAR(0.2, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties50degrees)
{
    constexpr auto angle = 50.0;

    auto angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance =
      angularSample->getProperty(lowLambda, highLambda, Property::T, Side::Front, angle);
    EXPECT_NEAR(0.31814747164539497, transmittance, 1e-6);

    auto reflectanceFront =
      angularSample->getProperty(lowLambda, highLambda, Property::R, Side::Front, angle);
    EXPECT_NEAR(0.48228836526994262, reflectanceFront, 1e-6);

    auto reflectanceBack =
      angularSample->getProperty(lowLambda, highLambda, Property::R, Side::Back, angle);
    EXPECT_NEAR(0.48228836526994262, reflectanceBack, 1e-6);

    auto absorptance =
      angularSample->getProperty(lowLambda, highLambda, Property::Abs, Side::Front, angle);
    EXPECT_NEAR(0.19956416308466268, absorptance, 1e-6);
}
