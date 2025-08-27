#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

// Example (test case) of sample that calculates angular properties of single layer sample

class TestSampleNFRC_103_Angular : public testing::Test
{
    const double thickness{5.715e-3};
    CAngularSpectralSample m_Sample{
      {SpectralSample::NFRC_103(), StandardData::solarRadiationASTM_E891_87_Table1()},
      thickness,
      MaterialType::Monolithic};

public:
    CAngularSpectralSample& getSample()
    {
        return m_Sample;
    }
};

TEST_F(TestSampleNFRC_103_Angular, TestProperties0degrees)
{
    auto angle = 0.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.77068014770698934, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.069984173508366929, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.070250681323265077, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.15933567878464375, absorptance, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular, TestProperties10degrees)
{
    constexpr auto angle = 10.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.76980319121439578, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.069950630413725984, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.070217043956113862, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.16024617837187857, absorptance, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular, TestProperties20degrees)
{
    constexpr auto angle = 20.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.76691124365416619, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.070135520990238370, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.070401987567688062, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.16295323535559544, absorptance, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular, TestProperties30degrees)
{
    constexpr auto angle = 30.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.76103766815923068, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.071607090478364152, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.071874821559627849, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.16735524136240523, absorptance, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular, TestProperties40degrees)
{
    constexpr auto angle = 40.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.74979572701044594, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.077031089090913732, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.077303147565901148, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.17317318389864053, absorptance, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular, TestProperties50degrees)
{
    constexpr auto angle = 50.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.72747576073993681, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.092802977512975046, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.093084622790465060, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.17972126174708844, absorptance, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular, TestProperties60degrees)
{
    constexpr auto angle = 60.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.68021254285214183, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.13439623099689185, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.13469298221439049, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.18539122615096648, absorptance, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular, TestProperties70degrees)
{
    constexpr auto angle = 70.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.57483609737842067, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.23889023227286083, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.23919930021548919, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.18627367034871853, absorptance, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular, TestProperties80degrees)
{
    constexpr auto angle = 80.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.34587632310986616, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.48399936961291273, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.48427878594468138, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.17012430727722111, absorptance, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular, TestProperties90degrees)
{
    constexpr auto angle = 90.0;

    auto & angularSample{getSample()};

    // SOLAR RANGE
    constexpr auto lowLambda = 0.3;
    constexpr auto highLambda = 2.5;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(1, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(1, reflectanceBack, 1e-6);

    auto absorptance = angularSample.getProperty(
      lowLambda, highLambda, Property::Abs, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0., absorptance, 1e-6);
}
