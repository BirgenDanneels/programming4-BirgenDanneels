#include "LoadingStructs.h"
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

namespace dae
{

    static ParamMap ParseParams(const Json& params)
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
}