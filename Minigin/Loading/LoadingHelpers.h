#pragma once
#include "LoadingStructs.h"
#include <nlohmann/json.hpp>
#include <stdexcept>

using Json = nlohmann::json;

namespace dae
{
    template <typename T>
    inline T GetRequiredParam(const ParamMap& params, const std::string& key)
    {
        auto it = params.find(key);
        if (it == params.end())
        {
            throw std::runtime_error("Missing required parameter: " + key);
        }

		// Check if the variant (Value) holds the expected type T before trying to get it.
        if (!std::holds_alternative<T>(it->second))
        {
            throw std::runtime_error("Invalid parameter type for: " + key);
        }

        return std::get<T>(it->second);
    }

    template <typename T>
    inline T GetOptionalParam(const ParamMap& params, const std::string& key, const T& defaultValue)
    {
        auto it = params.find(key);
        if (it != params.end() && std::holds_alternative<T>(it->second))
        {
            return std::get<T>(it->second);
        }
        return defaultValue;
    }

}