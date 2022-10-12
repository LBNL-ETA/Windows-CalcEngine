#include <gtest/gtest.h>

#include <WCETarcog.hpp>
#include <WCECommon.hpp>

class TestDeflectionDataInterpolation : public testing::Test
{

protected:
    void SetUp() override
    {}

public:
};

TEST_F(TestDeflectionDataInterpolation, InterpolationAtMidPoint)
{
    constexpr double Tolerance{1e-6};
    const auto tbl{DeflectionData::getWNData()};
    constexpr auto interpolationValue{2.5};
    const auto col{columnInterpolation(tbl, interpolationValue)};

    const std::vector correctX{-5.0, -2.6, -0.2, 2.2, 4.6, 7.0};
    const std::vector correctY{-4.256942, -1.85765, 0.472934, 2.082485, 3.144553, 4.691426};

    EXPECT_EQ(6u, col.size());
    for(unsigned i = 0u; i < correctY.size(); ++i)
    {
        EXPECT_NEAR(col[i].x.value(), correctX[i], Tolerance);
        EXPECT_NEAR(col[i].y.value(), correctY[i], Tolerance);
    }
}

TEST_F(TestDeflectionDataInterpolation, InterpolationAtStartPoint)
{
    constexpr double Tolerance{1e-6};
    const auto tbl{DeflectionData::getWNData()};
    constexpr auto interpolationValue{1.0};
    const auto col{columnInterpolation(tbl, interpolationValue)};

    const std::vector correctX{-5.0, -2.6, -0.2, 2.2, 4.6, 7.0};
    const std::vector correctY{-5.296, -2.8966, -0.5569, 1.067, 2.1892, 3.2125};

    EXPECT_EQ(6u, col.size());
    for(unsigned i = 0u; i < correctY.size(); ++i)
    {
        EXPECT_NEAR(col[i].x.value(), correctX[i], Tolerance);
        EXPECT_NEAR(col[i].y.value(), correctY[i], Tolerance);
    }
}

TEST_F(TestDeflectionDataInterpolation, InterpolationAtEndPoint)
{
    constexpr double Tolerance{1e-6};
    const auto tbl{DeflectionData::getWNData()};
    constexpr auto interpolationValue{10.0};
    const auto col{columnInterpolation(tbl, interpolationValue)};

    const std::vector correctX{-5.0, -2.6, -0.2, 2.2, 4.6, 7.0};
    const std::vector correctY{-4.1207, -1.7207, 0.6846, 3.1262, 4.7056, 6.23315};

    EXPECT_EQ(6u, col.size());
    for(unsigned i = 0u; i < correctY.size(); ++i)
    {
        EXPECT_NEAR(col[i].x.value(), correctX[i], Tolerance);
        EXPECT_NEAR(col[i].y.value(), correctY[i], Tolerance);
    }
}