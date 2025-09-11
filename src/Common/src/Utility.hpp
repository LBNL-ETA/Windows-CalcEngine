#pragma once

#include <vector>

namespace FenestrationCommon
{
    struct IndexRange
    {
        IndexRange(size_t startIndex, size_t endIndex);
        size_t start{0u};
        size_t end{0u};
    };

    //! Makes division for indexes that are defined from start to end for the purpose of
    //! multithreading.
    std::vector<IndexRange> chunkIt(size_t start, size_t end, size_t numberOfSplits);

    size_t getNumberOfThreads(size_t numberOfJobs);

    template<typename Map>
    bool map_compare(Map const & lhs, Map const & rhs)
    {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    [[nodiscard]] bool isVacuum(double pressure);
}   // namespace FenestrationCommon