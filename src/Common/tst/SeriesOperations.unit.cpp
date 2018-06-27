#include <memory>
#include <gtest/gtest.h>

#include "WCECommon.hpp"


using namespace FenestrationCommon;

class TestSeriesOperations : public testing::Test
{
private:
    std::unique_ptr<CSeries> m_Series1;
    std::unique_ptr<CSeries> m_Series2;

protected:
    void SetUp() override
    {
        m_Series1 = wce::make_unique<CSeries>();

        m_Series1->addProperty(0.50, 3.3);
        m_Series1->addProperty(0.51, 5.2);
        m_Series1->addProperty(0.52, 8.9);
        m_Series1->addProperty(0.53, 10.1);
        m_Series1->addProperty(0.54, 11.4);

        m_Series2 = wce::make_unique<CSeries>();

        m_Series2->addProperty(0.50, 1.2);
        m_Series2->addProperty(0.51, 6.1);
        m_Series2->addProperty(0.52, 7.3);
        m_Series2->addProperty(0.53, 9.5);
        m_Series2->addProperty(0.54, 10.4);
    }

public:
    CSeries* getSeries1() const {
        return m_Series1.get();
    };

    CSeries* getSeries2() const {
        return m_Series2.get();
    };
};

TEST_F(TestSeriesOperations, TestSeriesMultiplication)
{
    SCOPED_TRACE("Begin Test: Test multiplication over the range of data.");

    const auto ser1 = getSeries1();
    const auto ser2 = getSeries2();

    auto result = *ser1->mMult(*ser2);

    std::vector<double> correctResults{3.96, 31.72, 64.97, 95.95, 118.56};

    EXPECT_EQ(result.size(), correctResults.size());

    for(size_t i = 0; i < result.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], result[i].value(), 1e-6);
    }
}

TEST_F(TestSeriesOperations, TestSeriesAddition)
{
    SCOPED_TRACE("Begin Test: Test addition over the range of data.");

    const auto ser1 = getSeries1();
    const auto ser2 = getSeries2();

    auto result = *ser1->mAdd(*ser2);

    std::vector<double> correctResults{ 4.5, 11.3, 16.2, 19.6, 21.8 };

    EXPECT_EQ(result.size(), correctResults.size());

    for (size_t i = 0; i < result.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], result[i].value(), 1e-6);
    }
}

TEST_F(TestSeriesOperations, TestSeriesSubraction)
{
    SCOPED_TRACE("Begin Test: Test subraction over the range of data.");

    const auto ser1 = getSeries1();
    const auto ser2 = getSeries2();

    auto result = *ser1->mSub(*ser2);

    std::vector<double> correctResults{ 2.1, -0.9, 1.6, 0.6, 1 };

    EXPECT_EQ(result.size(), correctResults.size());

    for (size_t i = 0; i < result.size(); ++i)
    {
        EXPECT_NEAR(correctResults[i], result[i].value(), 1e-6);
    }
}