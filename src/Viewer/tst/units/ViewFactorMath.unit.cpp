#include <utility>
#include <vector>
#include <gtest/gtest.h>

#include "WCEViewer.hpp"
#include "../../src/ViewFactorMath.hpp"


using namespace Viewer;

// Parity tests: the lifted Viewer::detail free functions must reproduce the original
// CViewSegment2D methods exactly. This is the oracle that makes the lift-to-pure refactor
// safe; the new enclosure engine builds on these free functions.

namespace
{
    std::vector<std::pair<CViewSegment2D, CViewSegment2D>> samplePairs()
    {
        return {
          {CViewSegment2D{{0.0, 0.0}, {0.0, 1.0}}, CViewSegment2D{{1.0, 0.0}, {1.0, 1.0}}},
          {CViewSegment2D{{0.0, 0.0}, {0.0, 1.0}}, CViewSegment2D{{0.0, 1.0}, {1.0, 1.0}}},
          {CViewSegment2D{{0.0, 0.0}, {1.0, 0.0}}, CViewSegment2D{{0.0, 1.0}, {1.0, 1.0}}},
          {CViewSegment2D{{0.0, 0.0}, {2.0, 1.0}}, CViewSegment2D{{3.0, 0.0}, {1.0, 2.0}}},
          {CViewSegment2D{{-1.0, -1.0}, {1.0, -1.0}}, CViewSegment2D{{1.0, 1.0}, {-1.0, 1.0}}},
        };
    }
}   // namespace

TEST(ViewFactorMathParity, CrossStringCoefficient)
{
    for(const auto & [segA, segB] : samplePairs())
    {
        EXPECT_NEAR(segA.viewFactorCoefficient(segB),
                    detail::crossStringCoefficient(segA, segB),
                    1e-12);
    }
}

TEST(ViewFactorMathParity, SelfShadow)
{
    for(const auto & [segA, segB] : samplePairs())
    {
        EXPECT_EQ(segA.selfShadowing(segB), detail::selfShadow(segA, segB));
        EXPECT_EQ(segB.selfShadowing(segA), detail::selfShadow(segB, segA));
    }
}

TEST(ViewFactorMathParity, PointPosition)
{
    const std::vector<CPoint2D> probes{
      {0.5, 0.5}, {-1.0, 0.5}, {0.0, 0.0}, {0.0, 1.0}, {0.0, 0.3}, {2.0, -3.0}};

    for(const auto & [segA, segB] : samplePairs())
    {
        for(const auto & probe : probes)
        {
            EXPECT_EQ(segA.position(probe), detail::pointPosition(segA, probe));
            EXPECT_EQ(segB.position(probe), detail::pointPosition(segB, probe));
        }
    }
}

TEST(ViewFactorMathParity, Subdivide)
{
    for(const auto & [segA, segB] : samplePairs())
    {
        const auto memberSubs = segA.subSegments(ViewerConstants::NUM_OF_SEGMENTS);
        const auto freeSubs = detail::subdivide(segA, ViewerConstants::NUM_OF_SEGMENTS);

        ASSERT_EQ(memberSubs.size(), freeSubs.size());
        for(size_t idx = 0; idx < memberSubs.size(); ++idx)
        {
            EXPECT_TRUE(memberSubs[idx].startPoint().sameCoordinates(freeSubs[idx].startPoint()));
            EXPECT_TRUE(memberSubs[idx].endPoint().sameCoordinates(freeSubs[idx].endPoint()));
        }
    }
}
