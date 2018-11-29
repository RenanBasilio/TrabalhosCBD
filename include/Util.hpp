#pragma once
#include <string>
#include <vector>

std::vector<std::string> splitString(std::string str, char delimiter);

bool SNtoBool(const std::string& str);

constexpr int32_t constexpr_ceil(float num)
{
    return (static_cast<float>(static_cast<int32_t>(num)) == num)
        ? static_cast<int32_t>(num)
        : static_cast<int32_t>(num) + ((num > 0) ? 1 : 0);
}

