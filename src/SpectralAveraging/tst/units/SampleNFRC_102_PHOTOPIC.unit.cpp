#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestSampleNFRC_102_PHOTOPIC : public testing::Test
{
    std::shared_ptr<CSpectralSample> m_Sample;

protected:

    void SetUp() override
    {
        auto solarRadiation{StandardData::Photopic::solarRadiation()};
        const auto aWavelengths{StandardData::Photopic::wavelengthSetPhotopic()};
        const auto detectorData{StandardData::Photopic::detectorData()};
        auto sampleMeasurements{SpectralSample::NFRC_102()};

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

TEST_F(TestSampleNFRC_102_PHOTOPIC, TestSampleProperties)
{
    constexpr auto lowLambda = 0.38;
    constexpr auto highLambda = 0.78;

    auto aSample = getSample();
    const auto transmittance =
      aSample->getProperty(lowLambda, highLambda, Property::T, Side::Front);
    EXPECT_NEAR(0.899260, transmittance, 1e-6);

    const auto reflectanceFront =
      aSample->getProperty(lowLambda, highLambda, Property::R, Side::Front);
    EXPECT_NEAR(0.082563, reflectanceFront, 1e-6);

    const auto reflectanceBack =
      aSample->getProperty(lowLambda, highLambda, Property::R, Side::Back);
    EXPECT_NEAR(0.082563, reflectanceBack, 1e-6);
}
