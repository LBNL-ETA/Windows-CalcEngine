#include <memory>
#include <gtest/gtest.h>

#include <WCECommon.hpp>

TEST(TestSimpleTrapezoidalBIntegration, TestTrapezoidalB)
{
    SCOPED_TRACE("Begin Test: Test trapezoidal B integrator");

    using FenestrationCommon::CSeries;
    using FenestrationCommon::IntegrationType;

    const CSeries input{{10, 20}, {15, 30}, {20, 40}};
    const auto series = integrate(IntegrationType::TrapezoidalB, input);

    const CSeries correctValues{{10, 187.5}, {15, 262.5}};

    ASSERT_EQ(correctValues.size(), series.size());
    for (std::size_t i = 0; i < correctValues.size(); ++i)
    {
        EXPECT_NEAR(correctValues[i].x(),     series[i].x(),     1e-6);
        EXPECT_NEAR(correctValues[i].value(), series[i].value(), 1e-6);
    }
}