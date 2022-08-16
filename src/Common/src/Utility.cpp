#include "Utility.hpp"

namespace FenestrationCommon
{
    IndexRange::IndexRange(unsigned startIndex, unsigned endIndex) :
        start(startIndex), end(endIndex)
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
}   // namespace FenestrationCommon