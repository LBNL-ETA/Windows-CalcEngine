#include <memory>
#include <stdexcept>
#include <gtest/gtest.h>

#include "WCECommon.hpp"


using namespace FenestrationCommon;

class TestSPChipInterpolation : public testing::Test
{
private:
    std::shared_ptr<IInterpolation2D> m_Interpolation;

protected:
    void SetUp() override
    {
        std::vector<std::pair<double, double>> aPoints = {std::make_pair(24, 0.683876),
                                                          std::make_pair(34, 0.631739),
                                                          std::make_pair(48, 0.532746),
                                                          std::make_pair(62, 0.410234),
                                                          std::make_pair(75, 0.330733)};

        m_Interpolation = std::make_shared<CSPChipInterpolation2D>(aPoints);
    }

    std::shared_ptr<IInterpolation2D> getInterpolation() const
    {
        return m_Interpolation;
    };
};

TEST_F(TestSPChipInterpolation, TestInterpolations)
{
    SCOPED_TRACE("Begin Test: Interpolation in various ranges.");

    constexpr auto tolerance{1e-6};

    const std::shared_ptr<IInterpolation2D> aInterpolation = getInterpolation();

    double value = 28;
    value = aInterpolation->getValue(value);
    EXPECT_NEAR(value, 0.664845, tolerance);

    value = 40.9106;
    value = aInterpolation->getValue(value);
    EXPECT_NEAR(value, 0.586155, tolerance);

    value = 20;
    value = aInterpolation->getValue(value);
    EXPECT_NEAR(value, 0.683876, tolerance);

    value = 80;
    value = aInterpolation->getValue(value);
    EXPECT_NEAR(value, 0.330733, tolerance);
}
