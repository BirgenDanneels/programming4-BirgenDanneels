#pragma once

#include <variant>
#include <unordered_map>
#include <string>
#include <vector>
#include "Minigin/Events/Observer.h"

namespace dae
{
    using Value = std::variant<int, float, bool, std::string, std::vector<int>>;
    using ParamMap = std::unordered_map<std::string, Value>;

    struct ParamDefinition
    {
        std::string name;
        Value defaultValue;
    };

    struct LinkRequest
    {
        IObserver* observerPtr;
        std::string targetGameObjectName;
        std::string targetComponentName;
        std::string eventName;
    };
}
