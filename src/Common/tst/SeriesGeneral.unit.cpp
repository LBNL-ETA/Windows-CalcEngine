#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"


using namespace FenestrationCommon;

class TestSeriesGeneral : public testing::Test
{
private:
    CSeries m_Series;

protected:
    void SetUp() override
    {
        const std::vector<std::pair<double, double>> vec{
          {0.5, 3.3}, {0.51, 5.2}, {0.52, 8.9}, {0.53, 10.1}, {0.54, 11.4}};

        m_Series = CSeries(vec);
    }

public:
    CSeries getSeries() const
    {
        return m_Series;
    };
};

TEST_F(TestSeriesGeneral, TestSeriesMultiplication)
{
    SCOPED_TRACE("Begin Test: Test multiplication over the range of data.");

    auto ser = getSeries();

    ser.insertToBeginning(0.55, 15.0);

    std::vector<double> correctResults{ 15.0, 3.3, 5.2, 8.9, 10.1, 11.4};

    EXPECT_EQ(ser.size(), correctResults.size());

    for(size_t i = 0; i < ser.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], ser[i].value(), 1e-6);
    }
}
