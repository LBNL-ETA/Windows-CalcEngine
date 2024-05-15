#pragma once

#include <vector>
#include <thread>
#include <algorithm>

#include "Utility.hpp"

namespace FenestrationCommon {
    template<typename IndexType, typename Function>
    void executeInParallel(IndexType start, IndexType end, Function&& func)
    {
        const auto numberOfThreads = getNumberOfThreads(end - start + 1);
        const auto chunks = chunkIt(start, end, numberOfThreads);

        std::vector<std::thread> workers;
        workers.reserve(chunks.size());

        for (const auto& chunk : chunks)
        {
            workers.emplace_back([&, chunk]() {
                for (IndexType i = chunk.start; i < chunk.end; ++i)
                {
                    func(i);
                }
            });
        }

        for (auto& worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }
}
