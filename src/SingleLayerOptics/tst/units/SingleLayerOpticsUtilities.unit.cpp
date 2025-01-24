#include <memory>
#include <gtest/gtest.h>

#include <WCESingleLayerOptics.hpp>

class TestSingleLayerOpticsUtilities : public testing::Test
{};

TEST_F(TestSingleLayerOpticsUtilities, BSDFDirectionEnumeration)
{
    using SingleLayerOptics::BSDFDirection;
    std::vector<BSDFDirection> directions;
    SingleLayerOptics::EnumBSDFDirection dirs;
    for(const auto & direction : dirs)
    {
        directions.emplace_back(direction);
    }

    const std::vector<BSDFDirection> correctDirections{BSDFDirection::Incoming,
                                                       BSDFDirection::Outgoing};

    EXPECT_EQ(correctDirections.size(), directions.size());
    for(size_t i = 0; i < correctDirections.size(); ++i)
    {
        EXPECT_EQ(correctDirections[i], directions[i]);
    }
}
