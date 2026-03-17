#include "HealthComponent.h"
#include "GameObject.h"

dae::HealthComponent::HealthComponent(GameObject& pOwner)
	: Component(pOwner), m_onDeadSubject()
{
}

dae::HealthComponent::~HealthComponent()
{
}

void dae::HealthComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::HealthComponent::Initialize(int health)
{
	m_Health = health;

	//Would be better if this was in an awake function 
	OnHealthChanged().NotifyObservers(m_Health);
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
