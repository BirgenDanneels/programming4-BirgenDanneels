#include "BarrelComponent.h"
#include <glm/glm.hpp>
#include "Minigin/Scene.h"
#include "Minigin/Physics/Rigidbody.h"
#include "Minigin/Physics/Collider.h"
#include "Bullet.h"
#include "Minigin/Sound/ServiceLocator.h"
#include "Minigin/Loading/LoadingHelpers.h"

BarrelComponent::BarrelComponent(dae::GameObject& pOwner)
	: Component(pOwner)
{
}

BarrelComponent::~BarrelComponent()
{
}

void BarrelComponent::Start()
{
	m_shotSoundId = dae::ServiceLocator::GetSoundSystem().LoadSound("Shot.wav");

	m_pTextureComponent = GetOwner()->GetComponent<dae::TextureComponent>();

	if(!m_pTextureComponent)
		m_pTextureComponent = GetOwner()->AddComponent<dae::TextureComponent>();

	m_pTextureComponent->Initialize("Barrel.png", false, -6, -15);

	GetOwner()->GetTransform().SetInheritRotation(false);
}

void BarrelComponent::Update(float deltaTime)
{
	if (m_shootTimer > 0)
		m_shootTimer -= deltaTime;

	m_rotationDirection = glm::clamp(m_rotationDirection, -1.0f, 1.0f);

	float startRotation = GetOwner()->GetTransform().GetLocalRotation();
	float rotatedAngle = startRotation + m_rotationDirection * m_rotationSpeed * deltaTime;

	GetOwner()->GetTransform().SetLocalRotation(rotatedAngle);
}

void BarrelComponent::Shoot()
{
	if (m_shootTimer > 0)
		return;

	//Shoot a bullet in the direction the barrel is facing
	float rotation = GetOwner()->GetTransform().GetWorldRotation();
	glm::vec2 direction = { cos(glm::radians(rotation)), sin(glm::radians(rotation)) };

	auto bullet = GetOwner()->GetScene()->CreateGameObject();
	bullet->GetTransform().SetWorldPosition(m_pBulletSpawnPoint->GetTransform().GetWorldPosition());
	bullet->GetTransform().SetWorldRotation(rotation);
	bullet->AddComponent<dae::TextureComponent>()->Initialize("PlayerBullet.png", true);
	bullet->AddComponent<dae::Collider>()->InitializeBoxCollider(8, 8);
	bullet->AddComponent<Bullet>()->Initialize(m_damage, m_maxBounces);
	auto rb  = bullet->AddComponent<dae::Rigidbody>();

	rb->Initialize(false, false, true);

	rb->SetVelocity(direction * 500.0f);

	//Shooting sound
	dae::ServiceLocator::GetSoundSystem().Play(m_shotSoundId, 0.2f);

	m_shootTimer = m_fireRate;
}

void BarrelComponent::Rotate(float direction)
{
	m_rotationDirection = direction;
}


void BarrelComponent::Initialize(float fireRate, dae::GameObject& bulletSpawn, int damage, int maxBounces, float rotationSpeed)
{
	m_fireRate = fireRate;
	m_pBulletSpawnPoint = &bulletSpawn;
	m_damage = damage;
	m_maxBounces = maxBounces;
	m_rotationSpeed = rotationSpeed;
}

std::vector<dae::ParamDefinition> BarrelComponent::GetExpectedParams() const
{
	dae::GameObject* nullPtr = nullptr;

	return { { "fireRate", 5.f },
		{ "bulletSpawn", nullPtr},
		{ "damage", 1 },
		{ "maxBounces", 5 },
		{ "rotationSpeed", 180.f } };
}

void BarrelComponent::Load(const dae::ParamMap& params)
{
	float shootCooldown = dae::GetRequiredParam<float>(params, "fireRate");
	dae::GameObject* bulletSpawn = dae::GetRequiredParam<dae::GameObject*>(params, "bulletSpawn");
	int damage = dae::GetRequiredParam<int>(params, "damage");
	int maxBounces = dae::GetRequiredParam<int>(params, "maxBounces");
	float rotationSpeed = dae::GetRequiredParam<float>(params, "rotationSpeed");

	Initialize(shootCooldown, *bulletSpawn, damage, maxBounces, rotationSpeed);
}
