#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "spectralSampleData.hpp"
#include "standardData.hpp"


using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestSampleNFRC_1042 : public testing::Test
{
    std::shared_ptr<CSpectralSample> m_Sample;

protected:
    void SetUp() override
    {
        const auto solarRadiation = StandardData::solarRadiationASTM_E891_87_Table1();

        const auto sampleMeasurements = SpectralSample::NFRC_1042();

        m_Sample = std::make_shared<CSpectralSample>(sampleMeasurements, solarRadiation);
    }

public:
    [[nodiscard]] std::shared_ptr<CSpectralSample> getSample() const
    {
        return m_Sample;
    };
};

TEST_F(TestSampleNFRC_1042, TestSampleEnergy)
{
    auto lowLambda = 0.3;
    auto highLambda = 2.5;

    auto aSample = getSample();
    auto transmittedSolar = aSample->getEnergy(lowLambda, highLambda, Property::T, Side::Front, ScatteringType::Total);
    EXPECT_NEAR(341.803618, transmittedSolar, 1e-6);

    aSample->setWavelengths(WavelengthSet::Source);
    transmittedSolar = aSample->getEnergy(lowLambda, highLambda, Property::T, Side::Front, ScatteringType::Total);
    EXPECT_NEAR(341.756495, transmittedSolar, 1e-6);

    // This is to test repeatability
    aSample->setWavelengths(WavelengthSet::Data);
    transmittedSolar = aSample->getEnergy(lowLambda, highLambda, Property::T, Side::Front, ScatteringType::Total);
    EXPECT_NEAR(341.803618, transmittedSolar, 1e-6);

    auto reflectedFrontSolar = aSample->getEnergy(lowLambda, highLambda, Property::R, Side::Front, ScatteringType::Total);
    EXPECT_NEAR(271.886233, reflectedFrontSolar, 1e-6);

    auto reflectedBackSolar = aSample->getEnergy(lowLambda, highLambda, Property::R, Side::Back, ScatteringType::Total);
    EXPECT_NEAR(300.413703, reflectedBackSolar, 1e-6);

    auto absorbedSolar = aSample->getEnergy(lowLambda, highLambda, Property::Abs, Side::Front, ScatteringType::Total);
    EXPECT_NEAR(143.124694, absorbedSolar, 1e-6);
}

TEST_F(TestSampleNFRC_1042, TestSampleProperties)
{
    auto lowLambda = 0.3;
    auto highLambda = 2.5;

    auto aSample = getSample();
    auto transmittance = aSample->getProperty(lowLambda, highLambda, Property::T, Side::Front);
    EXPECT_NEAR(0.451635, transmittance, 1e-6);

    auto reflectanceFront = aSample->getProperty(lowLambda, highLambda, Property::R, Side::Front);
    EXPECT_NEAR(0.359251, reflectanceFront, 1e-6);

    auto reflectanceBack = aSample->getProperty(lowLambda, highLambda, Property::R, Side::Back);
    EXPECT_NEAR(0.396945, reflectanceBack, 1e-6);

    auto absorptance = aSample->getProperty(lowLambda, highLambda, Property::Abs, Side::Front);
    EXPECT_NEAR(0.189115, absorptance, 1e-6);
}
