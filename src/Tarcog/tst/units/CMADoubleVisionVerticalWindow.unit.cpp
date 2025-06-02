#include <gtest/gtest.h>

#include "WCETarcog.hpp"

class TestCMADoubleVisionVerticalWindow : public testing::Test
{
protected:
    void SetUp() override
    {}
};

TEST_F(TestCMADoubleVisionVerticalWindow, CMADualVerticalVision)
{
    SCOPED_TRACE("Begin Test: CMA test for double vision vertical window.");

    constexpr Tarcog::ISO15099::FrameData frameDataBestBestHead{.Class = std::nullopt,
                                                                .UValue = 1.306919,
                                                                .EdgeUValue = 0.794668,
                                                                .ProjectedFrameDimension =
                                                                  0.042875183,
                                                                .WettedLength = 0.110605026};
    constexpr Tarcog::ISO15099::FrameData frameDataBestWorstHead{.Class = std::nullopt,
                                                                 .UValue = 1.65724,
                                                                 .EdgeUValue = 2.71409,
                                                                 .ProjectedFrameDimension =
                                                                   0.042875183,
                                                                 .WettedLength = 0.110605026};
    constexpr Tarcog::ISO15099::FrameData frameDataWorstBestHead{.Class = std::nullopt,
                                                                 .UValue = 2.27964,
                                                                 .EdgeUValue = 1.65214,
                                                                 .ProjectedFrameDimension =
                                                                   0.042875183,
                                                                 .WettedLength = 0.110605026};
    constexpr Tarcog::ISO15099::FrameData frameDataWorstWorstHead{.Class = std::nullopt,
                                                                  .UValue = 2.32377,
                                                                  .EdgeUValue = 3.19643,
                                                                  .ProjectedFrameDimension =
                                                                    0.042875183,
                                                                  .WettedLength = 0.110605026};

    const CMA::CMAFrame cmaFrameHead{frameDataBestBestHead,
                                     frameDataBestWorstHead,
                                     frameDataWorstBestHead,
                                     frameDataWorstWorstHead};

    constexpr Tarcog::ISO15099::FrameData frameDataBestBestJamb{.Class = std::nullopt,
                                                                .UValue = 1.25968,
                                                                .EdgeUValue = 0.76981,
                                                                .ProjectedFrameDimension =
                                                                  0.042875183,
                                                                .WettedLength = 0.110605026};
    constexpr Tarcog::ISO15099::FrameData frameDataBestWorstJamb{.Class = std::nullopt,
                                                                 .UValue = 1.62145,
                                                                 .EdgeUValue = 2.70202,
                                                                 .ProjectedFrameDimension =
                                                                   0.042875183,
                                                                 .WettedLength = 0.110605026};
    constexpr Tarcog::ISO15099::FrameData frameDataWorstBestJamb{.Class = std::nullopt,
                                                                 .UValue = 2.26579,
                                                                 .EdgeUValue = 1.64520,
                                                                 .ProjectedFrameDimension =
                                                                   0.042875183,
                                                                 .WettedLength = 0.110605026};
    constexpr Tarcog::ISO15099::FrameData frameDataWorstWorstJamb{.Class = std::nullopt,
                                                                  .UValue = 2.30879,
                                                                  .EdgeUValue = 3.18888,
                                                                  .ProjectedFrameDimension =
                                                                    0.042875183,
                                                                  .WettedLength = 0.110605026};

    const CMA::CMAFrame cmaFrameJamb{frameDataBestBestJamb,
                                     frameDataBestWorstJamb,
                                     frameDataWorstBestJamb,
                                     frameDataWorstWorstJamb};

    constexpr Tarcog::ISO15099::FrameData frameDataBestBestSill{.Class = std::nullopt,
                                                                .UValue = 1.30474,
                                                                .EdgeUValue = 0.79449,
                                                                .ProjectedFrameDimension =
                                                                  0.042875183,
                                                                .WettedLength = 0.110605026};
    constexpr Tarcog::ISO15099::FrameData frameDataBestWorstSill{.Class = std::nullopt,
                                                                 .UValue = 1.64813,
                                                                 .EdgeUValue = 2.71240,
                                                                 .ProjectedFrameDimension =
                                                                   0.042875183,
                                                                 .WettedLength = 0.110605026};
    constexpr Tarcog::ISO15099::FrameData frameDataWorstBestSill{.Class = std::nullopt,
                                                                 .UValue = 2.27038,
                                                                 .EdgeUValue = 1.64528,
                                                                 .ProjectedFrameDimension =
                                                                   0.042875183,
                                                                 .WettedLength = 0.110605026};
    constexpr Tarcog::ISO15099::FrameData frameDataWorstWorstSill{.Class = std::nullopt,
                                                                  .UValue = 2.31302,
                                                                  .EdgeUValue = 3.18880,
                                                                  .ProjectedFrameDimension =
                                                                    0.042875183,
                                                                  .WettedLength = 0.110605026};

    const CMA::CMAFrame cmaFrameSill{frameDataBestBestSill,
                                     frameDataBestWorstSill,
                                     frameDataWorstBestSill,
                                     frameDataWorstWorstSill};

    const auto width{1.2};
    const auto height{1.5};

    auto window{CMA::CMAWindowDualVisionVertical(width, height)};

    using Tarcog::ISO15099::DualVerticalFramePosition;
    window.setFrameData({{DualVerticalFramePosition::Top, cmaFrameHead},
                         {DualVerticalFramePosition::Bottom, cmaFrameSill},
                         {DualVerticalFramePosition::TopLeft, cmaFrameJamb},
                         {DualVerticalFramePosition::TopRight, cmaFrameJamb},
                         {DualVerticalFramePosition::BottomLeft, cmaFrameJamb},
                         {DualVerticalFramePosition::BottomRight, cmaFrameJamb},
                         {DualVerticalFramePosition::MeetingRail, cmaFrameJamb}});

    const auto UvalueCOG{1.258};
    const auto SHGCCOG{0.341};
    const auto tVis{0.535};
    const auto spacerKeff{0.750454253};

    const double vt{window.vt(tVis)};
    EXPECT_NEAR(0.454171, vt, 1e-6);

    const double uvalue{window.uValue(UvalueCOG, spacerKeff)};
    EXPECT_NEAR(1.511768, uvalue, 1e-6);

    const double windowSHGC{window.shgc(SHGCCOG, spacerKeff)};
    EXPECT_NEAR(0.290800, windowSHGC, 1e-6);

    const auto iguDimensions{window.getIGUDimensions()};
    EXPECT_NEAR(1.114250, iguDimensions.width, 1e-6);
    EXPECT_NEAR(0.685687, iguDimensions.height, 1e-6);
}
