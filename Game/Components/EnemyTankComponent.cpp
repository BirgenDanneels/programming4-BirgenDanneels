#include "EnemyTankComponent.h"

#include "HealthComponent.h"
#include "NodeMovementComponent.h"
#include "Bullet.h"

#include "Minigin/GameObject.h"
#include "Minigin/Scene.h"
#include "Minigin/Loading/LoadingHelpers.h"
#include "Minigin/Physics/Collider.h"
#include "Minigin/Physics/Rigidbody.h"
#include "Minigin/Sound/ServiceLocator.h"
#include "Minigin/Components/TextureComponent.h"
#include "Minigin/Utils/HashUtil.h"

#include <glm/gtc/random.hpp>

EnemyTankComponent::EnemyTankComponent(dae::GameObject& owner)
	: Component(owner)
{
	if (!GetOwner()->GetComponent<NodeMovementComponent>())
	{
		GetOwner()->AddComponent<NodeMovementComponent>();
	}
}

void EnemyTankComponent::Start()
{
	m_ShotSoundId = dae::ServiceLocator::GetSoundSystem().GetSoundId("Shot.wav");

	m_pHealthComponent = GetOwner()->GetComponent<HealthComponent>();
	m_pNodeMovementComponent = GetOwner()->GetComponent<NodeMovementComponent>();
	m_pOwnCollider = GetOwner()->GetComponent<dae::Collider>();

	if (!m_pHealthComponent)
		m_pHealthComponent = GetOwner()->AddComponent<HealthComponent>();

	if (!m_pNodeMovementComponent)
		m_pNodeMovementComponent = GetOwner()->AddComponent<NodeMovementComponent>();

	m_pHealthComponent->Initialize(m_Lives);
	m_pNodeMovementComponent->Initialize(m_Speed, m_pStartNode, m_NodeReachDistance);

	m_NodeReachedHandle = m_pNodeMovementComponent->OnNodeReached().AddObserver(this);

	if (m_pOwnCollider)
	{
		m_CollisionHandle = m_pOwnCollider->OnCollision().AddObserver(this);
	}

	DecideAtNode(m_pNodeMovementComponent->GetCurrentNode());
}

void EnemyTankComponent::Update(float deltaTime)
{
	if (m_ShootTimer > 0.f)
	{
		m_ShootTimer -= deltaTime;
	}

	if (m_CollisionInputCooldown > 0.f)
	{
		m_CollisionInputCooldown -= deltaTime;
	}

	if (CanSeePlayer())
	{
		Shoot();
	}
}

void EnemyTankComponent::OnNotify(dae::GameObject* reachedNode)
{
	DecideAtNode(reachedNode);
}

void EnemyTankComponent::OnNotify(dae::HitEvent hitEvent)
{
	if (!m_pNodeMovementComponent)
		return;

	if (m_CollisionInputCooldown > 0.f)
		return;

	dae::Collider* otherCollider = hitEvent.otherCollider;

	if (!otherCollider)
		return;

	dae::GameObject* other = otherCollider->GetOwner();

	if (!other)
		return;

	if (other->GetTag() != dae::make_sdbm_hash("Enemy"))
		return;

	if (!ShouldTurnAroundAfterEnemyCollision(other))
		return;

	const TankDirection currentDirection = m_pNodeMovementComponent->GetCurrentDirection();
	const TankDirection oppositeDirection = GetOppositeDirection(currentDirection);

	if (oppositeDirection == TankDirection::None)
		return;

	m_CollisionInputCooldown = m_CollisionInputCooldownDuration;

	m_pNodeMovementComponent->SetHeldDirection(oppositeDirection);
}

void EnemyTankComponent::DecideAtNode(dae::GameObject* node)
{
	if (!node || !m_pNodeMovementComponent)
		return;

	TankDirection direction = TankDirection::None;

	if (m_pTargetPlayer)
	{
		direction = GetBestDirectionTowardsPlayer(node);
	}
	else
	{
		direction = GetRandomValidDirection(node);
	}

	m_pNodeMovementComponent->SetHeldDirection(direction);
}

bool EnemyTankComponent::CanSeePlayer()
{
	if (!m_pOwnCollider)
		return false;

	const float rotation = GetOwner()->GetTransform().GetWorldRotation();

	const glm::vec2 direction
	{
		glm::cos(glm::radians(rotation)),
		glm::sin(glm::radians(rotation))
	};

	const glm::vec3 origin3 = GetOwner()->GetTransform().GetWorldPosition();
	const glm::vec2 origin{ origin3.x, origin3.y };

	dae::RaycastHit hit{};
	if (!dae::ServiceLocator::GetPhysicsManager().Raycast(origin, direction, m_VisionLength, hit, m_pOwnCollider))
		return false;

	if (!hit.collider)
		return false;

	if (hit.collider->GetOwner()->GetTag() == dae::make_sdbm_hash("Player"))
	{
		m_pTargetPlayer = hit.collider->GetOwner();
		return true;
	}

	return false;
}

void EnemyTankComponent::Shoot()
{
	if (m_ShootTimer > 0.f || !m_pBulletSpawnPoint)
		return;

	const float rotation = GetOwner()->GetTransform().GetWorldRotation();

	const glm::vec2 direction
	{
		glm::cos(glm::radians(rotation)),
		glm::sin(glm::radians(rotation))
	};

	auto bullet = GetOwner()->GetScene()->CreateGameObject();

	bullet->GetTransform().SetWorldPosition(
		m_pBulletSpawnPoint->GetTransform().GetWorldPosition()
	);

	bullet->GetTransform().SetWorldRotation(rotation);

	bullet->AddComponent<dae::TextureComponent>()->Initialize("PlayerBullet.png", true);
	bullet->AddComponent<dae::Collider>()->InitializeBoxCollider(8, 8);
	bullet->AddComponent<Bullet>()->Initialize(m_Damage, m_MaxBounces, "Player");

	auto rigidbody = bullet->AddComponent<dae::Rigidbody>();
	rigidbody->Initialize(false, false, true);
	rigidbody->SetVelocity(direction * 500.f);

	dae::ServiceLocator::GetSoundSystem().Play(m_ShotSoundId, 0.2f);

	m_ShootTimer = m_FireRate;
}

TankDirection EnemyTankComponent::GetRandomValidDirection(dae::GameObject* node) const
{
	const std::vector<TankDirection> validDirections = GetValidDirections(node);

	if (validDirections.empty())
		return TankDirection::None;

	const int index = glm::linearRand(0, static_cast<int>(validDirections.size()) - 1);
	return validDirections[index];
}

TankDirection EnemyTankComponent::GetBestDirectionTowardsPlayer(dae::GameObject* node) const
{
	if (!node || !m_pTargetPlayer)
		return GetRandomValidDirection(node);

	TankNodeComponent* nodeComponent = node->GetComponent<TankNodeComponent>();

	if (!nodeComponent)
		return TankDirection::None;

	const glm::vec3 playerPosition = m_pTargetPlayer->GetTransform().GetWorldPosition();

	TankDirection bestDirection = TankDirection::None;
	float bestDistanceSquared = std::numeric_limits<float>::max();

	for (TankDirection direction : GetValidDirections(node))
	{
		dae::GameObject* neighbour = nodeComponent->GetNeighbour(direction);

		if (!neighbour)
			continue;

		const glm::vec3 neighbourPosition = neighbour->GetTransform().GetWorldPosition();

		const float dx = neighbourPosition.x - playerPosition.x;
		const float dy = neighbourPosition.y - playerPosition.y;
		const float distanceSquared = dx * dx + dy * dy;

		if (distanceSquared < bestDistanceSquared)
		{
			bestDistanceSquared = distanceSquared;
			bestDirection = direction;
		}
	}

	if (bestDirection == TankDirection::None)
		return GetRandomValidDirection(node);

	return bestDirection;
}

TankDirection EnemyTankComponent::GetDifferentValidDirection(dae::GameObject* node, TankDirection badDirection) const
{
	std::vector<TankDirection> validDirections = GetValidDirections(node);

	if (validDirections.empty())
		return TankDirection::None;

	validDirections.erase(
		std::remove(validDirections.begin(), validDirections.end(), badDirection),
		validDirections.end()
	);

	const TankDirection opposite = GetOppositeDirection(badDirection);

	if (validDirections.size() > 1)
	{
		validDirections.erase(
			std::remove(validDirections.begin(), validDirections.end(), opposite),
			validDirections.end()
		);
	}

	if (validDirections.empty())
		return GetRandomValidDirection(node);

	const int index = glm::linearRand(0, static_cast<int>(validDirections.size()) - 1);
	return validDirections[index];
}

bool EnemyTankComponent::ShouldTurnAroundAfterEnemyCollision(dae::GameObject* otherEnemy) const
{
	if (!otherEnemy || !m_pNodeMovementComponent)
		return false;

	NodeMovementComponent* otherMovement = otherEnemy->GetComponent<NodeMovementComponent>();

	if (!otherMovement)
		return true;

	const TankDirection myDirection = m_pNodeMovementComponent->GetCurrentDirection();
	const TankDirection otherDirection = otherMovement->GetCurrentDirection();

	if (myDirection == TankDirection::None)
		return false;

	if (otherDirection == TankDirection::None)
		return true;

	const glm::vec2 myDirectionVector = DirectionToVector(myDirection);
	const glm::vec2 otherDirectionVector = DirectionToVector(otherDirection);

	const float directionDot = glm::dot(myDirectionVector, otherDirectionVector);

	if (directionDot < -0.5f)
		return true;

	const glm::vec3 myPosition3 = GetOwner()->GetTransform().GetWorldPosition();
	const glm::vec3 otherPosition3 = otherEnemy->GetTransform().GetWorldPosition();

	const glm::vec2 myPosition{ myPosition3.x, myPosition3.y };
	const glm::vec2 otherPosition{ otherPosition3.x, otherPosition3.y };

	const glm::vec2 fromOtherToMe = myPosition - otherPosition;

	// Only the enemy behind should turn around
	if (directionDot > 0.5f)
	{
		const float myForwardOffsetFromOther = glm::dot(fromOtherToMe, myDirectionVector);

		return myForwardOffsetFromOther < 0.f;
	}

	const glm::vec2 fromMeToOther = otherPosition - myPosition;
	const float movingIntoOther = glm::dot(fromMeToOther, myDirectionVector);

	return movingIntoOther > 0.f;
}

std::vector<TankDirection> EnemyTankComponent::GetValidDirections(dae::GameObject* node) const
{
	std::vector<TankDirection> validDirections;

	if (!node)
		return validDirections;

	TankNodeComponent* nodeComponent = node->GetComponent<TankNodeComponent>();

	if (!nodeComponent)
		return validDirections;

	constexpr TankDirection directions[]
	{
		TankDirection::Up,
		TankDirection::Down,
		TankDirection::Left,
		TankDirection::Right
	};

	for (TankDirection direction : directions)
	{
		if (nodeComponent->GetNeighbour(direction))
		{
			validDirections.push_back(direction);
		}
	}

	return validDirections;
}

glm::vec2 EnemyTankComponent::DirectionToVector(TankDirection direction) const
{
	switch (direction)
	{
	case TankDirection::Up:
		return { 0.f, -1.f };

	case TankDirection::Down:
		return { 0.f, 1.f };

	case TankDirection::Left:
		return { -1.f, 0.f };

	case TankDirection::Right:
		return { 1.f, 0.f };

	case TankDirection::None:
	default:
		return { 0.f, 0.f };
	}
}

TankDirection EnemyTankComponent::GetOppositeDirection(TankDirection direction) const
{
	switch (direction)
	{
	case TankDirection::Up:
		return TankDirection::Down;

	case TankDirection::Down:
		return TankDirection::Up;

	case TankDirection::Left:
		return TankDirection::Right;

	case TankDirection::Right:
		return TankDirection::Left;

	case TankDirection::None:
	default:
		return TankDirection::None;
	}
}

void EnemyTankComponent::Initialize(float speed, int lives, dae::GameObject& bulletSpawn, dae::GameObject* startNode, float nodeReachDistance, float visionLength, float fireRate, int damage, int maxBounces)
{
	m_Speed = speed;
	m_Lives = lives;
	m_pBulletSpawnPoint = &bulletSpawn;
	m_pStartNode = startNode;
	m_NodeReachDistance = nodeReachDistance;
	m_VisionLength = visionLength;
	m_FireRate = fireRate;
	m_Damage = damage;
	m_MaxBounces = maxBounces;

	if (m_pHealthComponent)
	{
		m_pHealthComponent->Initialize(m_Lives);
	}

	if (m_pNodeMovementComponent)
	{
		m_pNodeMovementComponent->Initialize(m_Speed, m_pStartNode, m_NodeReachDistance);
	}
}

std::vector<dae::ParamDefinition> EnemyTankComponent::GetExpectedParams() const
{
	dae::GameObject* nullPtr{ nullptr };

	return
	{
		{ "speed", 80.0f },
		{ "lives", 1 },
		{ "bulletSpawn", nullPtr },
		{ "startNode", nullPtr },
		{ "nodeReachDistance", 1.0f },
		{ "visionLength", 1000.0f },
		{ "fireRate", 1.25f },
		{ "damage", 1 },
		{ "maxBounces", 3 }
	};
}

void EnemyTankComponent::Load(const dae::ParamMap& params)
{
	const float speed = GetRequiredParam<float>(params, "speed");
	const int lives = GetRequiredParam<int>(params, "lives");

	dae::GameObject* bulletSpawn = GetRequiredParam<dae::GameObject*>(params, "bulletSpawn");
	dae::GameObject* startNode = GetRequiredParam<dae::GameObject*>(params, "startNode");

	const float nodeReachDistance = GetRequiredParam<float>(params, "nodeReachDistance");
	const float visionLength = GetRequiredParam<float>(params, "visionLength");
	const float fireRate = GetRequiredParam<float>(params, "fireRate");

	const int damage = GetRequiredParam<int>(params, "damage");
	const int maxBounces = GetRequiredParam<int>(params, "maxBounces");

	Initialize(speed, lives, *bulletSpawn, startNode, nodeReachDistance, visionLength, fireRate, damage, maxBounces);
}