#include "GameEventQueue.h"

#include <algorithm>
#include <utility>

dae::SubscriptionHandle dae::GameEventQueue::Subscribe(unsigned int id, Handler handler)
{
	const int subscriptionId{ m_nextSubscriptionId++ };
	m_subscribers[id].push_back(Subscription{ subscriptionId, std::move(handler)});
	return SubscriptionHandle{ id, subscriptionId, false };
}

dae::SubscriptionHandle dae::GameEventQueue::SubscribeAll(Handler handler)
{
	const int subscriptionId{ m_nextSubscriptionId++ };
	m_globalSubscribers.push_back(Subscription{ subscriptionId, std::move(handler)});
	return SubscriptionHandle{ 0, subscriptionId, true };

}

void dae::GameEventQueue::Unsubscribe(SubscriptionHandle handle)
{
	if (handle.isGlobal)
	{
		m_globalSubscribers.erase(
			std::remove_if(
				m_globalSubscribers.begin(),
				m_globalSubscribers.end(),
				[handle](const Subscription& subscription) { return subscription.id == handle.subscriptionId; }),
			m_globalSubscribers.end());
		return;
	}

	const auto it = m_subscribers.find(handle.eventId);
	if (it == m_subscribers.end()) return;

	auto& handlers = it->second;
	handlers.erase(
		std::remove_if(
			handlers.begin(),
			handlers.end(),
			[handle](const Subscription& subscription) { return subscription.id == handle.subscriptionId; }),
		handlers.end());

	if (handlers.empty()) m_subscribers.erase(it);
}

void dae::GameEventQueue::Enqueue(const Event& event)
{
	if (m_capacity == 0) return;

	if (m_count == m_capacity)
	{
		// Full: drop oldest
		m_head = (m_head + 1) % m_capacity;
		--m_count;
	}

	m_buffer[m_tail] = event;
	m_tail = (m_tail + 1) % m_capacity;
	++m_count;
}

void dae::GameEventQueue::Enqueue(Event&& event)
{
	if (m_capacity == 0) return;

	if (m_count == m_capacity)
	{
		// Full: drop oldest
		m_head = (m_head + 1) % m_capacity;
		--m_count;
	}

	m_buffer[m_tail] = std::move(event);
	m_tail = (m_tail + 1) % m_capacity;
	++m_count;
}

void dae::GameEventQueue::Dispatch()
{
	while (m_count > 0)
	{
		Event event = std::move(m_buffer[m_head]);
		m_head = (m_head + 1) % m_capacity;
		--m_count;

		const auto it = m_subscribers.find(event.id);
		if (it != m_subscribers.end())
		{
			for (const Subscription& subscription : it->second)
			{
				if (subscription.handler) subscription.handler(event);
			}
		}

		for (const Subscription& subscription : m_globalSubscribers)
		{
			if (subscription.handler) subscription.handler(event);
		}
	}
}

void dae::GameEventQueue::Clear()
{
	m_head = 0;
	m_tail = 0;
	m_count = 0;

	m_subscribers.clear();
	m_globalSubscribers.clear();
	m_nextSubscriptionId = 1;
}