#include "Bullet.h"
#include "Minigin/GameObject.h"
#include "Minigin/Utils/HashUtil.h"
#include "TankComponent.h"
#include "Minigin/Loading/LoadingHelpers.h"

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

std::vector<dae::ParamDefinition> Bullet::GetExpectedParams() const
{
	return { {"damage", 1}, {"bounces", 5} };
}

void Bullet::Load(const dae::ParamMap& params)
{
	m_damage = GetRequiredParam<int>(params, "damage");
	m_bouncesRemaining = GetRequiredParam<int>(params, "bounces");
}

void Bullet::Start()
{
	m_pCollider = GetOwner()->GetComponent<dae::Collider>();
	if (m_pCollider)
	{
		m_collisionHandle = m_pCollider->OnCollision().AddObserver(this);
	}
}

void Bullet::OnNotify(dae::HitEvent hit)
{
	auto otherObject = hit.otherCollider->GetOwner();
	if (otherObject->GetTag() == dae::make_sdbm_hash("Player"))
	{
		if (auto comp = otherObject->GetComponent<TankComponent>())
		{
			// Handle collision with tank
			comp->TakeDamage(m_damage);
			GetOwner()->Delete();
		}
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
