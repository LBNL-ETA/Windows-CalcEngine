#include <gtest/gtest.h>

#include <WCECommon.hpp>

class TestTableVectorInterpolation : public testing::Test
{
private:
    std::vector<Table::point> m_Table{{1,2}, {2,4}, {3,8}};

protected:
    void SetUp() override
    {}

public:

    [[nodiscard]] const std::vector<Table::point> & getTable() const
    {
        return m_Table;
    }
};

TEST_F(TestTableVectorInterpolation, InterpolationAtMidPoint)
{
    const auto tbl{getTable()};
    const auto interpolationValue{1.5};
    const auto val{Table::tableColumnInterpolation(tbl, interpolationValue)};
    const auto correctVal{3.0};

    ASSERT_EQ(val.has_value(), true);

    EXPECT_NEAR(val.value(), correctVal, 1e-6);
}

TEST_F(TestTableVectorInterpolation, InterpolationBeforeTheStartPoint)
{
    const auto tbl{getTable()};
    const auto interpolationValue{0.5};
    const auto val{Table::tableColumnInterpolation(tbl, interpolationValue)};

    EXPECT_EQ(val.has_value(), false);
}

TEST_F(TestTableVectorInterpolation, InterpolationAfterTheEndPoint)
{
    const auto tbl{getTable()};
    const auto interpolationValue{3.5};
    const auto val{Table::tableColumnInterpolation(tbl, interpolationValue)};

    EXPECT_EQ(val.has_value(), false);
}