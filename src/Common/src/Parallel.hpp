#pragma once

#include <vector>
#include <thread>
#include <algorithm>

#include "Utility.hpp"
#include "Callbacks.hpp"

namespace FenestrationCommon
{
    template<typename IndexType, typename Function>
    void executeInParallel(IndexType start,
                           IndexType end,
                           Function && func,
                           ProgressCallback progressCb = nullptr)
    {
        const IndexType total = end - start + 1;
        const auto numberOfThreads = getNumberOfThreads(total);
        //const auto numberOfThreads = 1;
        const auto chunks = chunkIt(start, end, numberOfThreads);

        std::atomic<IndexType> progressCounter{0};
        std::vector<std::thread> workers;
        workers.reserve(chunks.size());

        for(const auto & chunk : chunks)
        {
            workers.emplace_back([&, chunk]() {
                for(IndexType i = chunk.start; i < chunk.end; ++i)
                {
                    func(i);

                    if(progressCb)
                    {
                        IndexType current = ++progressCounter;
                        progressCb(current, total);
                    }
                }
            });
        }

        for(auto & worker : workers)
        {
            if(worker.joinable())
            {
                worker.join();
            }
        }
    }
}   // namespace FenestrationCommon
