#include "HealthComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/Loading/LoadingHelpers.h"

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
