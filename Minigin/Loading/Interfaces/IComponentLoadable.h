#pragma once
#include "Minigin/Loading/LoadingStructs.h"
#include "Minigin/Loading/LoadingHelpers.h"

namespace dae
{

    class IComponentLoadable
    {
    public:
        virtual ~IComponentLoadable() = default;

        virtual std::vector<ParamDefinition> GetExpectedParams() const = 0;
        virtual void Load(const ParamMap& params) = 0;
    };

}