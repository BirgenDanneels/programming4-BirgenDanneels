#include <string>
#include "Events/Observer.h"

namespace dae
{
    class IEventLinkable
    {
    public:
        virtual ~IEventLinkable() = default;

        virtual bool Bind(const std::string& eventName, IObserver* observer) = 0;
        virtual bool Unbind(const std::string& eventName, IObserver* observer) = 0;

    };
}