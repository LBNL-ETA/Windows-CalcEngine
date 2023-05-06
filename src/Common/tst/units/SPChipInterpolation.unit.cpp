#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCECommon.hpp"


using namespace FenestrationCommon;

class TestSPChipInterpolation : public testing::Test
{};

TEST_F(TestSPChipInterpolation, TestInterpolations)
{
    SCOPED_TRACE("Begin Test: Interpolation in various ranges.");

    constexpr auto tolerance{1e-6};

    SPChipInterpolation2D aInterpolation(
      {{24, 0.683876}, {34, 0.631739}, {48, 0.532746}, {62, 0.410234}, {75, 0.330733}});

    double value = 28;
    value = aInterpolation.value(value);
    EXPECT_NEAR(value, 0.664845, tolerance);

    value = 40.9106;
    value = aInterpolation.value(value);
    EXPECT_NEAR(value, 0.586155, tolerance);

    value = 20;
    value = aInterpolation.value(value);
    EXPECT_NEAR(value, 0.683876, tolerance);

    value = 80;
    value = aInterpolation.value(value);
    EXPECT_NEAR(value, 0.330733, tolerance);
}
