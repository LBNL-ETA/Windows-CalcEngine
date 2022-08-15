#include <vector>
#include <gtest/gtest.h>

#include "WCECommon.hpp"

using namespace FenestrationCommon;

class TestChunkIt : public testing::Test
{};

TEST_F(TestChunkIt, Chunk1)
{
    constexpr size_t start{0u};
    constexpr size_t end{3u};
    constexpr size_t numberOfSplits{2u};

    const auto result{FenestrationCommon::chunkIt(start, end, numberOfSplits)};
    const std::vector<FenestrationCommon::IndexRange> correct{{0u, 1u}, {2u, 3u}};

    EXPECT_EQ(result.size(), correct.size());
    for(size_t i = 0u; i < correct.size(); ++i)
    {
        EXPECT_EQ(result[i].start, correct[i].start);
        EXPECT_EQ(result[i].end, correct[i].end);
    }
}

TEST_F(TestChunkIt, Chunk2)
{
    constexpr size_t start{0u};
    constexpr size_t end{3u};
    constexpr size_t numberOfSplits{12u};

    const auto result{FenestrationCommon::chunkIt(start, end, numberOfSplits)};
    const std::vector<FenestrationCommon::IndexRange> correct{
      {0u, 0u}, {1u, 1u}, {2u, 2u}, {3u, 3u}};

    EXPECT_EQ(result.size(), correct.size());
    for(size_t i = 0u; i < correct.size(); ++i)
    {
        EXPECT_EQ(result[i].start, correct[i].start);
        EXPECT_EQ(result[i].end, correct[i].end);
    }
}

TEST_F(TestChunkIt, Chunk3)
{
    constexpr size_t start{1u};
    constexpr size_t end{4u};
    constexpr size_t numberOfSplits{2u};

    const auto result{FenestrationCommon::chunkIt(start, end, numberOfSplits)};
    const std::vector<FenestrationCommon::IndexRange> correct{{1u, 2u}, {3u, 4u}};

    EXPECT_EQ(result.size(), correct.size());
    for(size_t i = 0u; i < correct.size(); ++i)
    {
        EXPECT_EQ(result[i].start, correct[i].start);
        EXPECT_EQ(result[i].end, correct[i].end);
    }
}

TEST_F(TestChunkIt, Chunk4)
{
    constexpr size_t start{3u};
    constexpr size_t end{5u};
    constexpr size_t numberOfSplits{12u};

    const auto result{FenestrationCommon::chunkIt(start, end, numberOfSplits)};
    const std::vector<FenestrationCommon::IndexRange> correct{{3u, 3u}, {4u, 4u}, {5u, 5u}};

    EXPECT_EQ(result.size(), correct.size());
    for(size_t i = 0u; i < correct.size(); ++i)
    {
        EXPECT_EQ(result[i].start, correct[i].start);
        EXPECT_EQ(result[i].end, correct[i].end);
    }
}

TEST_F(TestChunkIt, Chunk5)
{
    constexpr size_t start{5u};
    constexpr size_t end{5u};
    constexpr size_t numberOfSplits{12u};

    const auto result{FenestrationCommon::chunkIt(start, end, numberOfSplits)};
    const std::vector<FenestrationCommon::IndexRange> correct{{5u, 5u}};

    EXPECT_EQ(result.size(), correct.size());
    for(size_t i = 0u; i < correct.size(); ++i)
    {
        EXPECT_EQ(result[i].start, correct[i].start);
        EXPECT_EQ(result[i].end, correct[i].end);
    }
}