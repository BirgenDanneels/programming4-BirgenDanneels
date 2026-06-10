#include "Bullet.h"
#include "Minigin/GameObject.h"
#include "Minigin/Utils/HashUtil.h"
#include "HealthComponent.h"
#include "Minigin/Loading/LoadingHelpers.h"

Bullet::Bullet(dae::GameObject& pOwner)
	: Component(pOwner)
{
}

Bullet::~Bullet()
{
}

void Bullet::Initialize(int damage, int bounces, std::string targetTag)
{
	m_damage = damage;
	m_bouncesRemaining = bounces;
	m_targetTag = std::move(targetTag);
}

std::vector<dae::ParamDefinition> Bullet::GetExpectedParams() const
{
	return { {"damage", 1}, {"bounces", 5}, {"targetTag", "Default"} };
}

void Bullet::Load(const dae::ParamMap& params)
{
	m_damage = GetRequiredParam<int>(params, "damage");
	m_bouncesRemaining = GetRequiredParam<int>(params, "bounces");
	m_targetTag = GetRequiredParam<std::string>(params, "targetTag");
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
	if (otherObject->GetTag() == dae::make_sdbm_hash_runtime(m_targetTag.c_str()))
	{
		if (auto comp = otherObject->GetComponent<HealthComponent>())
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
