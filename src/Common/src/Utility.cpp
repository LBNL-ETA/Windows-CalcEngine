#include <thread>
#include <algorithm>
#include "Utility.hpp"

namespace FenestrationCommon
{
    IndexRange::IndexRange(unsigned startIndex, unsigned endIndex) :
        start(startIndex),
        end(endIndex)
    {}

    std::vector<IndexRange> chunkIt(unsigned start, unsigned end, unsigned numberOfSplits)
    {
        unsigned stepSize{numberOfSplits < (end - start)
                            ? static_cast<unsigned>((end - start) / numberOfSplits)
                            : 0u};

        std::vector<IndexRange> result;

        unsigned currentStart{start};
        unsigned currentEnd{currentStart};

        do
        {
            currentEnd = currentStart + stepSize < end ? currentStart + stepSize + 1u : end + 1u;
            result.emplace_back(currentStart, currentEnd);
            currentStart = currentEnd;
        } while(currentEnd < end + 1u);

        return result;
    }

    size_t getNumberOfThreads(size_t numberOfJobs)
    {
        size_t numberOfThreads{std::min<size_t>(std::thread::hardware_concurrency(), static_cast<size_t>(numberOfJobs / 2))};
        //static const size_t maxNumberOfThreads{32u};
        //numberOfThreads = std::min(numberOfThreads, maxNumberOfThreads);
        static const size_t minNumberOfThreads{1u};
        return std::max(minNumberOfThreads, numberOfThreads);
    }
}   // namespace FenestrationCommon