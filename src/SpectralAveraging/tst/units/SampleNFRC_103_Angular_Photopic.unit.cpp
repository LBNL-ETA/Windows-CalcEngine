#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestSampleNFRC_103_Angular_Photopic : public testing::Test
{
    CSpectralSample m_Sample{SpectralSample::NFRC_103(), StandardData::Photopic::solarRadiation()};

protected:
    void SetUp() override
    {
        m_Sample.setDetectorData(StandardData::Photopic::detectorData());
        m_Sample.setWavelengths(WavelengthSet::Custom, StandardData::Photopic::wavelengthSetPhotopic());
    }

public:
    CSpectralSample getSample() const
    {
        return m_Sample;
    };
};

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties0degrees)
{
    auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 0.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.883647, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.080395, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.080395, reflectanceBack, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties10degrees)
{
    constexpr auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 10.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.883411, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.080401, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.080401, reflectanceBack, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties20degrees)
{
    constexpr auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 20.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.882397, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.080729, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.080729, reflectanceBack, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties30degrees)
{
    constexpr auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 30.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.879453, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.082542, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.082542, reflectanceBack, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties40degrees)
{
    constexpr auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 40.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.871710, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.088749, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.088750, reflectanceBack, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties50degrees)
{
    constexpr auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 50.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.852261, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.106358, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.106358, reflectanceBack, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties60degrees)
{
    constexpr auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 60.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.804351, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.152339, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.152339, reflectanceBack, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties70degrees)
{
    constexpr auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 70.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.688205, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.266908, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.266908, reflectanceBack, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties80degrees)
{
    constexpr auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 80.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.427264, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0.527714, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(0.527714, reflectanceBack, 1e-6);
}

TEST_F(TestSampleNFRC_103_Angular_Photopic, TestProperties90degrees)
{
    constexpr auto thickness = 5.715e-3;   // [m]
    auto layerType = MaterialType::Monolithic;

    constexpr auto angle = 90.0;

    auto aMeasuredSample = getSample();

    CAngularSpectralSample angularSample{aMeasuredSample, thickness, layerType};

    // VISIBLE (PHOTOPIC) RANGE
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto transmittance = angularSample.getProperty(
      lowLambda, highLambda, Property::T, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(0, transmittance, 1e-6);

    auto reflectanceFront = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Front, angle, ScatteringType::Total);
    EXPECT_NEAR(1, reflectanceFront, 1e-6);

    auto reflectanceBack = angularSample.getProperty(
      lowLambda, highLambda, Property::R, Side::Back, angle, ScatteringType::Total);
    EXPECT_NEAR(1, reflectanceBack, 1e-6);
}
