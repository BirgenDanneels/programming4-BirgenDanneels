#pragma once
#include <variant>
#include <unordered_map>
#include <string>
#include <vector>
#include "Minigin/Events/Observer.h"

namespace dae
{
    class GameObject; // Forward declaration

    using Value = std::variant<int, float, bool, std::string, std::vector<int>, std::vector<std::string>, GameObject*, std::vector<GameObject*>>;
    using ParamMap = std::unordered_map<std::string, Value>;

    struct ParamDefinition
    {
        std::string name;
        Value defaultValue;
    };
}
