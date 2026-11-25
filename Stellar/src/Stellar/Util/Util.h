#pragma once
#include <random>

namespace Stellar {

    inline int GetRandomInt(const std::vector<int>& values) {
        static std::mt19937 rng(std::random_device{}());

        std::uniform_int_distribution<size_t> dist(0, values.size() - 1);

        return values[dist(rng)];
    }


}