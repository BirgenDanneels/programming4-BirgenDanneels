#include "Bullet.h"
#include "Minigin/GameObject.h"
#include "TankComponent.h"

Bullet::Bullet(dae::GameObject& pOwner)
	: Component(pOwner)
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize(int damage, int bounces)
{
	m_damage = damage;
	m_bouncesRemaining = bounces;
}

void Bullet::Start()
{
	m_pCollider = GetOwner()->GetComponent<dae::Collider>();
	if (m_pCollider)
	{
		m_pCollider->OnCollision().AddObserver(this);
	}
}

void Bullet::Update(float deltaTime)
{
	(void)deltaTime;
}

void Bullet::OnNotify(dae::Hit hit)
{
	auto otherCollider = (hit.a == m_pCollider) ? hit.b : hit.a;

	//IMPLEMENT TAGS

	auto otherObject = otherCollider->GetOwner();
	if(auto comp = otherObject->GetComponent<TankComponent>())
	{
		// Handle collision with tank
		comp->TakeDamage(m_damage);
		GetOwner()->Delete();
	}
	else
	{
		// Handle collision with wall or other object
		if (m_bouncesRemaining <= 0)
		{
			GetOwner()->Delete();
		}

		m_bouncesRemaining--;
	}
}
