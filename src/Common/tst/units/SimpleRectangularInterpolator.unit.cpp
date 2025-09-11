#include <memory>
#include <gtest/gtest.h>

#include <WCECommon.hpp>

TEST(TestSimpleRectangularIntegration, TestRectangular)
{
    SCOPED_TRACE("Begin Test: Test rectangular integrator");

    using FenestrationCommon::CSeries;
    using FenestrationCommon::IntegrationType;

    const CSeries input{{10, 20}, {15, 30}, {20, 40}};
    const auto series = integrate(IntegrationType::Rectangular, input);

    const CSeries correctValues{{10, 100}, {15, 150}};

    ASSERT_EQ(correctValues.size(), series.size());
    for(std::size_t i = 0; i < correctValues.size(); ++i)
    {
        EXPECT_NEAR(correctValues[i].x(), series[i].x(), 1e-6);
        EXPECT_NEAR(correctValues[i].value(), series[i].value(), 1e-6);
    }
}
