#pragma once

#include <vector>

namespace FenestrationCommon
{
    struct IndexRange
    {
        IndexRange(unsigned startIndex, unsigned endIndex);
        unsigned start{0u};
        unsigned end{0u};
    };

    //! Makes division for indexes that are defined from start to end for the purpose of multithreading.
    std::vector<IndexRange> chunkIt(unsigned start, unsigned end, unsigned numberOfSplits);

    size_t getNumberOfThreads(size_t numberOfJobs);
}