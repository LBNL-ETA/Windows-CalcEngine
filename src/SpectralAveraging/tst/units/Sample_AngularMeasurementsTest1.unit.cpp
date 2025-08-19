#include <memory>
#include <gtest/gtest.h>

#include <WCESpectralAveraging.hpp>
#include <WCECommon.hpp>

#include "standardData.hpp"

using namespace SpectralAveraging;
using namespace FenestrationCommon;

// Example (test case) of sample that calculates angular properties of single layer sample with
// linear interpolation

class Sample_AngularMeasurementsTest1 : public testing::Test
{
    std::shared_ptr<CAngularMeasurements> m_Measurements;

    static std::shared_ptr<CSingleAngularMeasurement> getSample1()
    {
        auto aSolarRadiation = StandardData::solarRadiationASTM_E891_87_Table1();

        auto aMeasurements40 = CSpectralSampleData::create({
          {0.290, {0.10, 0.10, 0.1, 0.1}, {0.0, 0.0, 0.0, 0.0}},
          {0.295, {0.15, 0.15, 0.099, 0.099}, {0.0, 0.0, 0.0, 0.0}},
          {0.300, {0.20, 0.20, 0.098, 0.098}, {0.0, 0.0, 0.0, 0.0}},
          {0.310, {0.25, 0.25, 0.097, 0.097}, {0.0, 0.0, 0.0, 0.0}},
        });

        auto aSample40 = std::make_shared<CSpectralSample>(aMeasurements40, aSolarRadiation);
        auto aAngular40 = std::make_shared<CSingleAngularMeasurement>(aSample40, 40);

        return aAngular40;
    }

    static std::shared_ptr<CSingleAngularMeasurement> getSample2()
    {
        auto aSolarRadiation = StandardData::solarRadiationASTM_E891_87_Table1();

        auto aMeasurements50 = CSpectralSampleData::create({
          {0.290, {0.15, 0.15, 0.1, 0.10}, {0.0, 0.0, 0.0, 0.0}},
          {0.295, {0.20, 0.20, 0.098, 0.099}, {0.0, 0.0, 0.0, 0.0}},
          {0.305, {0.25, 0.25, 0.097, 0.098}, {0.0, 0.0, 0.0, 0.0}},
          {0.310, {0.30, 0.30, 0.096, 0.097}, {0.0, 0.0, 0.0, 0.0}},
        });

        auto aSample50 = std::make_shared<CSpectralSample>(aMeasurements50, aSolarRadiation);
        auto aAngular50 = std::make_shared<CSingleAngularMeasurement>(aSample50, 50);

        return aAngular50;
    }


public:
    std::shared_ptr<CAngularMeasurements> getMeasurements() const
    {
        return m_Measurements;
    }

protected:
    void SetUp() override
    {
        auto aAngular40 = getSample1();
        auto aAngular50 = getSample2();

        // Need to extract common wavelengths
        CCommonWavelengths aCommonWL;
        auto wl40 = aAngular40->getWavelengthsFromSample();
        auto wl50 = aAngular50->getWavelengthsFromSample();
        aCommonWL.addWavelength(wl40);
        aCommonWL.addWavelength(wl50);
        auto commonWavelengths = aCommonWL.getCombinedWavelengths(Combine::Interpolate);

        // Creating angular sample
        m_Measurements = std::make_shared<CAngularMeasurements>(aAngular40, commonWavelengths);
        m_Measurements->addMeasurement(aAngular50);
    }
};

TEST_F(Sample_AngularMeasurementsTest1, TestProperties45degrees)
{
    constexpr auto angle = 45.0;

    auto aMeasurements = getMeasurements();

    auto aAngleMeasurement = aMeasurements->getMeasurements(angle);

    auto aSample = aAngleMeasurement->getData();

    // Now retrieve specific properties

    // Front transmittances
    auto aTransmittances = aSample->getWavelengthsProperty(Property::T, Side::Front);

    std::vector<double> correctT = {0.125, 0.175, 0.2125, 0.2375, 0.275};

    auto size = aTransmittances.size();

    EXPECT_EQ(size, correctT.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctT[i], aTransmittances[i].value(), 1e-6);
    }

    // Front reflectances
    auto aFReflectances = aSample->getWavelengthsProperty(Property::R, Side::Front);

    std::vector<double> correctFR = {0.1, 0.0985, 0.09775, 0.09725, 0.0965};

    size = aFReflectances.size();

    EXPECT_EQ(size, correctFR.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctFR[i], aFReflectances[i].value(), 1e-6);
    }

    // Back reflectances
    auto aBReflectances = aSample->getWavelengthsProperty(Property::R, Side::Back);

    std::vector<double> correctBR = {0.1, 0.099, 0.09825, 0.09775, 0.097};

    size = aBReflectances.size();

    EXPECT_EQ(size, correctBR.size());
    for(size_t i = 0; i < size; ++i)
    {
        EXPECT_NEAR(correctBR[i], aBReflectances[i].value(), 1e-6);
    }
}
