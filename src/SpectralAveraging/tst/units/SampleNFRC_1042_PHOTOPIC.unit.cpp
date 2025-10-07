#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "../../../helper/optical/standardData.hpp"
#include "../../../helper/optical/spectralSampleData.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestSampleNFRC_1042_PHOTOPIC : public testing::Test
{
    std::shared_ptr<CSpectralSample> m_Sample;

protected:
    void SetUp() override
    {
        auto solarRadiation{StandardData::Photopic::solarRadiation()};
        auto aWavelengths{StandardData::Photopic::wavelengthSetPhotopic()};
        auto detectorData{StandardData::Photopic::detectorData()};
        auto sampleMeasurements{SpectralSample::NFRC_1042()};

        m_Sample = std::make_shared<CSpectralSample>(sampleMeasurements, solarRadiation);
        m_Sample->setDetectorData(detectorData);

        m_Sample->setWavelengths(WavelengthSet::Custom, aWavelengths);
    }

public:
    std::shared_ptr<CSpectralSample> getSample() const
    {
        return m_Sample;
    };
};

TEST_F(TestSampleNFRC_1042_PHOTOPIC, TestSampleProperties)
{
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto aSample = getSample();
    auto transmittance = aSample->getProperty(lowLambda, highLambda, Property::T, Side::Front);
    EXPECT_NEAR(0.714001, transmittance, 1e-6);

    auto reflectanceFront = aSample->getProperty(lowLambda, highLambda, Property::R, Side::Front);
    EXPECT_NEAR(0.207224, reflectanceFront, 1e-6);

    auto reflectanceBack = aSample->getProperty(lowLambda, highLambda, Property::R, Side::Back);
    EXPECT_NEAR(0.147697, reflectanceBack, 1e-6);
}
