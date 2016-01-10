#pragma once
#include <vector>

struct Storage
{
    std::vector<int> get(const std::string& key)
    {
        return {1, 2, 3, 4};
    }
};
