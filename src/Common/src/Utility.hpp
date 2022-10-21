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

    //! Makes division for indexes that are defined from start to end for the purpose of multithreading.
    std::vector<IndexRange> chunkIt(size_t start, size_t end, size_t numberOfSplits);

    size_t getNumberOfThreads(size_t numberOfJobs);
}