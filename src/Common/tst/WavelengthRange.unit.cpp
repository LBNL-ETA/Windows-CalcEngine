#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"


using namespace FenestrationCommon;

class WavelengthRangeTest : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(WavelengthRangeTest, Test1)
{
    SCOPED_TRACE("Begin Test: Creation of IR range.");

    CWavelengthRange aRange = CWavelengthRange(WavelengthRange::IR);

    EXPECT_NEAR(5.0, aRange.minLambda(), 1e-6);
    EXPECT_NEAR(100.0, aRange.maxLambda(), 1e-6);
}

TEST_F(WavelengthRangeTest, Test2)
{
    SCOPED_TRACE("Begin Test: Creation of Solar range.");

    CWavelengthRange aRange = CWavelengthRange(WavelengthRange::Solar);

    EXPECT_NEAR(0.3, aRange.minLambda(), 1e-6);
    EXPECT_NEAR(2.5, aRange.maxLambda(), 1e-6);
}

TEST_F(WavelengthRangeTest, Test3)
{
    SCOPED_TRACE("Begin Test: Creation of Visible range.");

    CWavelengthRange aRange = CWavelengthRange(WavelengthRange::Visible);

    EXPECT_NEAR(0.38, aRange.minLambda(), 1e-6);
    EXPECT_NEAR(0.78, aRange.maxLambda(), 1e-6);
}

TEST_F(WavelengthRangeTest, TestCondensedSpectrum)
{
    SCOPED_TRACE("Begin Test: Creation of condensed spectrum range.");

    const size_t numOfVisibleBands{5u};
    const size_t numOfIRBands{10u};

    const auto condensendSpectrum =
      FenestrationCommon::generateSpectrum(numOfVisibleBands, numOfIRBands);

    std::vector<double> correctSpectrum{0.3,
                                        0.38,
                                        0.46,
                                        0.54,
                                        0.62,
                                        0.7,
                                        0.78,
                                        0.952,
                                        1.124,
                                        1.296,
                                        1.468,
                                        1.64,
                                        1.812,
                                        1.984,
                                        2.156,
                                        2.328,
                                        2.5};

    EXPECT_EQ(condensendSpectrum.size(), correctSpectrum.size());
    for(size_t i = 0u; i < condensendSpectrum.size(); ++i)
    {
        EXPECT_NEAR(condensendSpectrum[i], correctSpectrum[i], 1e-6);
    }
}
