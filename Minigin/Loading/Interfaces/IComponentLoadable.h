#pragma once
#include "Loading/LoadingStructs.h"

namespace dae
{

    class IComponentLoadable
    {
    public:
        virtual ~IComponentLoadable() = default;

        virtual void Load(const ParamMap& params) = 0;
    };

}