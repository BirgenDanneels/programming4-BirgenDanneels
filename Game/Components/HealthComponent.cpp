#include "HealthComponent.h"
#include "Minigin/GameObject.h"
#include <stdexcept>

HealthComponent::HealthComponent(dae::GameObject& pOwner)
	: dae::Component(pOwner), m_onDeadSubject()
{
}

HealthComponent::~HealthComponent()
{
}

void HealthComponent::Start()
{
	OnHealthChanged().NotifyObservers(m_Health);
}

void HealthComponent::Initialize(int health)
{
	m_Health = health;
}

void HealthComponent::TakeDamage(int damage)
{
	m_Health -= damage;
	if (m_Health <= 0)
	{
		m_Health = 0;
		m_onDeadSubject.NotifyObservers(GetOwner());
	}

	m_onHealthChangedSubject.NotifyObservers(m_Health);
}

std::vector<dae::ParamDefinition> HealthComponent::GetExpectedParams() const
{
	return {
		{ "health", 3 }
	};
}

void HealthComponent::Load(const dae::ParamMap& params)
{
	int health = dae::GetRequiredParam<int>(params, "health");

	Initialize(health);
}

bool HealthComponent::Bind(const std::string& eventName, dae::IObserver* observer)
{
	if (eventName == "OnHealthChanged")
	{
		if (auto* obs = static_cast<dae::Observer<int>*>(observer))
		{
			OnHealthChanged().AddObserver(obs);
			return true;
		}
	}
	else if (eventName == "OnDead")
	{
		if (auto* obs = static_cast<dae::Observer<dae::GameObject*>*>(observer))
		{
			OnDead().AddObserver(obs);
			return true;
		}
	}

	return false;
}

bool HealthComponent::Unbind(const std::string& eventName, dae::IObserver* observer)
{
	if (eventName == "OnHealthChanged")
	{
		if (auto* obs = dynamic_cast<dae::Observer<int>*>(observer))
		{
			OnHealthChanged().RemoveObserver(obs);
			return true;
		}
	}
	else if (eventName == "OnDead")
	{
		if (auto* obs = dynamic_cast<dae::Observer<dae::GameObject*>*>(observer))
		{
			OnDead().RemoveObserver(obs);
			return true;
		}
	}
	return false;
}
