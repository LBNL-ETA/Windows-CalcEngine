#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"


using namespace FenestrationCommon;

class TestSimpleTrapezoidalBIntegration : public testing::Test
{
private:
    std::shared_ptr<IIntegratorStrategy> m_Integrator;

protected:
    void SetUp() override
    {
        CIntegratorFactory aFactory = CIntegratorFactory();
        m_Integrator = aFactory.getIntegrator(IntegrationType::TrapezoidalB);
    }

public:
    IIntegratorStrategy * getIntegrator() const
    {
        return m_Integrator.get();
    };
};

TEST_F(TestSimpleTrapezoidalBIntegration, TestTrapezoidalB)
{
    SCOPED_TRACE("Begin Test: Test trapezoidal B integrator");

    auto aIntegrator = getIntegrator();

    std::vector<CSeriesPoint> input{{10, 20}, {15, 30}, {20, 40}};

    const auto series = aIntegrator->integrate(input);

    CSeries correctValues{{10, 187.5}, {15, 262.5}};

    EXPECT_EQ(correctValues.size(), series.size());

    for(auto i = 0u; i < correctValues.size(); ++i)
    {
        EXPECT_NEAR(correctValues[i].x(), series[i].x(), 1e-6);
        EXPECT_NEAR(correctValues[i].value(), series[i].value(), 1e-6);
    }
}
