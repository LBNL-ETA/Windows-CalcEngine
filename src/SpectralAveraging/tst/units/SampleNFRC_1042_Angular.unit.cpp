#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"
#include "spectralSampleData.hpp"


using namespace SpectralAveraging;
using namespace FenestrationCommon;

class TestSampleNFRC_1042_Angular : public testing::Test
{
    std::shared_ptr<CSpectralSample> m_Sample;

protected:

    void SetUp() override
    {
        auto aSolarRadiation = StandardData::solarRadiationASTM_E891_87_Table1();

        auto aMeasurements = SpectralSample::NFRC_1042();

        m_Sample = std::make_shared<CSpectralSample>(aMeasurements, aSolarRadiation);
    }

public:
    [[nodiscard]] std::shared_ptr<CSpectralSample> getSample() const
    {
        return m_Sample;
    };
};

TEST_F(TestSampleNFRC_1042_Angular, TestSampleProperties)
{
    auto lowLambda = 0.3;
    auto highLambda = 2.5;

    auto thickness = 3.18e-3;   // [m]

    auto layerType = MaterialType::Coated;

    auto angle = 70.0;

    auto aMeasuredSample = getSample();

    auto angularSample =
      std::make_shared<CAngularSpectralSample>(aMeasuredSample, thickness, layerType);

    auto transmittance =
      angularSample->getProperty(lowLambda, highLambda, Property::T, Side::Front, angle);
    EXPECT_NEAR(0.312605, transmittance, 1e-6);

    auto reflectanceFront =
      angularSample->getProperty(lowLambda, highLambda, Property::R, Side::Front, angle);
    EXPECT_NEAR(0.460105, reflectanceFront, 1e-6);

    auto reflectanceBack =
      angularSample->getProperty(lowLambda, highLambda, Property::R, Side::Back, angle);
    EXPECT_NEAR(0.491866, reflectanceBack, 1e-6);

    auto absorptance =
      angularSample->getProperty(lowLambda, highLambda, Property::Abs, Side::Front, angle);
    EXPECT_NEAR(0.227290, absorptance, 1e-6);
}
