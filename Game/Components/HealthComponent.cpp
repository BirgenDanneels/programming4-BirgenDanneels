#include "HealthComponent.h"
#include "Minigin/GameObject.h"
#include <stdexcept>

dae::HealthComponent::HealthComponent(GameObject& pOwner)
	: Component(pOwner), m_onDeadSubject()
{
}

dae::HealthComponent::~HealthComponent()
{
}

void dae::HealthComponent::Start()
{
	OnHealthChanged().NotifyObservers(m_Health);
}

void dae::HealthComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::HealthComponent::Initialize(int health)
{
	m_Health = health;
}

void dae::HealthComponent::TakeDamage(int damage)
{
	m_Health -= damage;
	if (m_Health <= 0)
	{
		m_Health = 0;
		m_onDeadSubject.NotifyObservers(GetOwner());
	}

	m_onHealthChangedSubject.NotifyObservers(m_Health);
}

std::vector<dae::ParamDefinition> dae::HealthComponent::GetExpectedParams() const
{
	return {
		{ "health", 3 }
	};
}

void dae::HealthComponent::Load(const ParamMap& params)
{
	int health = GetRequiredParam<int>(params, "health");

	Initialize(health);
}

bool dae::HealthComponent::Bind(const std::string& eventName, IObserver* observer)
{
	if (eventName == "OnHealthChanged")
	{
		if (auto* obs = static_cast<Observer<int>*>(observer))
		{
			OnHealthChanged().AddObserver(obs);
			return true;
		}
	}
	else if (eventName == "OnDead")
	{
		if (auto* obs = static_cast<Observer<GameObject*>*>(observer))
		{
			OnDead().AddObserver(obs);
			return true;
		}
	}

	return false;
}

bool dae::HealthComponent::Unbind(const std::string& eventName, IObserver* observer)
{
	if (eventName == "OnHealthChanged")
	{
		if (auto* obs = dynamic_cast<Observer<int>*>(observer))
		{
			OnHealthChanged().RemoveObserver(obs);
			return true;
		}
	}
	else if (eventName == "OnDead")
	{
		if (auto* obs = dynamic_cast<Observer<GameObject*>*>(observer))
		{
			OnDead().RemoveObserver(obs);
			return true;
		}
	}
	return false;
}
