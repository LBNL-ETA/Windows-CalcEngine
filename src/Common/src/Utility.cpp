#include "Utility.hpp"

namespace FenestrationCommon
{
    IndexRange::IndexRange(size_t start, size_t end) : start(start), end(end)
    {}

    std::vector<IndexRange> chunkIt(size_t start, size_t end, size_t numberOfSplits)
    {
        size_t numOfLoops{numberOfSplits < (end - start)
                            ? static_cast<size_t>((end - start) / numberOfSplits) + 1u
                            : end - start + 1u};
        size_t stepSize{numberOfSplits < (end - start) ? numOfLoops - 1u : 0u};

        std::vector<IndexRange> result;
        result.reserve(numOfLoops);

        size_t currentStart{start};
        for(size_t i = 0u; i < numOfLoops; ++i)
        {
            size_t currentEnd{currentStart + stepSize};
            result.emplace_back(currentStart, currentEnd);
            currentStart = currentEnd + 1u;
        }
        return result;
    }
}   // namespace FenestrationCommon