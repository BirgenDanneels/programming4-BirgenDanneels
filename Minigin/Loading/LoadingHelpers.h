#pragma once
#include "LoadingStructs.h"
#include <nlohmann/json.hpp>
#include <stdexcept>

using Json = nlohmann::json;

namespace dae
{

    inline ParamMap ParseParams(const Json& params)
    {
        ParamMap out;

        if (!params.is_object())
            return out;

        for (auto it = params.begin(); it != params.end(); ++it)
        {
            const std::string key = it.key();
            const Json& value = it.value();

            if (value.is_string())
                out[key] = value.get<std::string>();
            else if (value.is_number_float())
                out[key] = value.get<float>();
            else if (value.is_number_integer())
                out[key] = value.get<int>();
            else if (value.is_boolean())
                out[key] = value.get<bool>();
            else if (value.is_array())
                out[key] = value.get<std::vector<int>>();
        }

        return out;
    }

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