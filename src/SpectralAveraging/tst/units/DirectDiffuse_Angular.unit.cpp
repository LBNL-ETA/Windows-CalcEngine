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
        auto layerType = MaterialType::Coated;

        m_Sample = std::make_shared<CAngularSpectralSample>(aSample, thickness, layerType);
    }

public:
    [[nodiscard]] std::shared_ptr<CAngularSpectralSample> getSample() const
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

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.3, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.5, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.5, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.2, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties0degreesDirect)
{
    constexpr auto angle{0.0};

    std::shared_ptr<CAngularSpectralSample> angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Direct);
    EXPECT_NEAR(0.1, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Direct);
    EXPECT_NEAR(0.2, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Direct);
    EXPECT_NEAR(0.2, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Direct);
    EXPECT_NEAR(0.2, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties0degreesDiffuse)
{
    constexpr auto angle{0.0};

    std::shared_ptr<CAngularSpectralSample> angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Diffuse);
    EXPECT_NEAR(0.2, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Diffuse);
    EXPECT_NEAR(0.3, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Diffuse);
    EXPECT_NEAR(0.3, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Diffuse);
    EXPECT_NEAR(0.2, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties10degrees)
{
    constexpr auto angle{10.0};

    auto angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.301978, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.495871, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.495871, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.202150, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties20degrees)
{
    constexpr auto angle{20.0};

    auto angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.298126, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.495000, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.495000, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.206874, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties30degrees)
{
    constexpr auto angle{30.0};

    auto angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.293440, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.496713, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.496713, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.209847, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties40degrees)
{
    constexpr auto angle{40.0};

    auto angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.287900, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.502344, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.502344, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.209757, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties50degrees)
{
    constexpr auto angle{50.0};

    auto angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.277791, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.511924, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.511924, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.210285, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties60degrees)
{
    constexpr auto angle{60.0};

    auto angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.254859, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.530266, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.530266, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.214875, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties70degrees)
{
    constexpr auto angle{70.0};

    auto angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.207649, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.578700, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.578700, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.213651, absorptance, 1e-6);
}

TEST_F(TestDirectDiffuse_Angular, TestProperties82dot5degrees)
{
    constexpr auto angle{82.5};

    auto angularSample = getSample();

    // SOLAR RANGE
    constexpr auto lowLambda{0.3};
    constexpr auto highLambda{2.5};

    auto transmittance = angularSample->getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.097600, transmittance, 1e-6);

    auto reflectanceFront = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.761130, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample->getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.761130, reflectanceBack, 1e-6);

    auto absorptance = angularSample->getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.141270, absorptance, 1e-6);
}