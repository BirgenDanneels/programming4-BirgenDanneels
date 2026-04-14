#pragma once
// Instead of using this interface, I think it could be better to use a std::function as callback. 
// This way there would be no inheritance and one class can observe multiple subjects without needing to implement multiple observer interfaces.
// Consider returning a subscription object that automatically unsubscribes when it goes out of scope (RAII).

namespace dae
{
	class IObserver
	{
	public:
		virtual ~IObserver() = default;
	};

	template<typename... Args>
	class Observer : public IObserver
	{
	public:
		virtual ~Observer() = default;
		virtual void OnNotify(Args... args) = 0;
	};
}
