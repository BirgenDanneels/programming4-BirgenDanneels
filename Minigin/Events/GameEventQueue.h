#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include "Event.h"

namespace dae
{
	struct SubscriptionHandle final
	{
		unsigned int eventId{};
		int subscriptionId{};
		bool isGlobal{};
	};

	class GameEventQueue final
	{
	public:
		using Handler = std::function<void(const Event&)>;

		SubscriptionHandle Subscribe(EventID id, Handler handler);
		//This should be made so the handler cannot change game state
		SubscriptionHandle SubscribeAll(Handler handler);
		void Unsubscribe(SubscriptionHandle handle);



		void Enqueue(const Event& event);
		void Enqueue(Event&& event);

		void Dispatch();
		void Clear();

	private:
		struct Subscription final
		{
			int id{};
			Handler handler{};
		};

		static constexpr int DefaultQueueCapacity{ 256 };

		std::vector<Event> m_buffer{ DefaultQueueCapacity };
		int m_head{ 0 };
		int m_tail{ 0 };
		int m_count{ 0 };
		int m_capacity{ DefaultQueueCapacity };

		std::unordered_map<EventID, std::vector<Subscription>> m_subscribers{};
		std::vector<Subscription> m_globalSubscribers{};
		int m_nextSubscriptionId{ 1 };
	};
}