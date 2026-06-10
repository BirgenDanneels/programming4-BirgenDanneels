#pragma once

#include "Minigin/Components/Component.h"
#include "Minigin/Events/Observer.h"
#include "Minigin/Events/Subject.h"

#include <glm/glm.hpp>
#include <vector>

class HealthComponent;
class NodeMovementComponent;

namespace dae
{
	class GameObject;
	struct HitEvent;
	class ObserverHandle;
	class Collider;
}

enum class TankDirection;

class EnemyTankComponent final: public dae::Component, public dae::Observer<dae::GameObject*>, public dae::Observer<dae::HitEvent>
{
public:
	explicit EnemyTankComponent(dae::GameObject& owner);
	~EnemyTankComponent() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void FixedUpdate(float) override {};
	void Render() const override {};

	void Initialize(
		float speed,
		int lives,
		dae::GameObject& player,
		dae::GameObject* startNode,
		float nodeReachDistance,
		float visionLength,
		float fireRate,
		int damage,
		int maxBounces
	);

	std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	void Load(const dae::ParamMap& params) override;

	void OnNotify(dae::GameObject* reachedNode) override;
	void OnNotify(dae::HitEvent hitEvent) override;

private:
	void DecideAtNode(dae::GameObject* node);

	bool CanSeePlayer();
	void Shoot();

	TankDirection GetRandomValidDirection(dae::GameObject* node) const;
	TankDirection GetBestDirectionTowardsPlayer(dae::GameObject* node) const;
	TankDirection GetDifferentValidDirection(dae::GameObject* node, TankDirection badDirection) const;

	bool ShouldTurnAroundAfterEnemyCollision(dae::GameObject* otherEnemy) const;

	std::vector<TankDirection> GetValidDirections(dae::GameObject* node) const;

	glm::vec2 DirectionToVector(TankDirection direction) const;
	TankDirection GetOppositeDirection(TankDirection direction) const;

private:
	float m_Speed{ 80.f };
	int m_Lives{ 1 };

	float m_NodeReachDistance{ 1.f };
	float m_VisionLength{ 1000.f };

	float m_FireRate{ 1.25f };
	float m_ShootTimer{ 0.f };

	float m_CollisionInputCooldown{ 0.f };
	float m_CollisionInputCooldownDuration{ 0.25f };

	int m_Damage{ 1 };
	int m_MaxBounces{ 3 };

	dae::GameObject* m_pTargetPlayer{ nullptr };
	dae::GameObject* m_pBulletSpawnPoint{ nullptr };
	dae::GameObject* m_pStartNode{ nullptr };

	HealthComponent* m_pHealthComponent{ nullptr };
	NodeMovementComponent* m_pNodeMovementComponent{ nullptr };
	dae::Collider* m_pOwnCollider{ nullptr };

	dae::ObserverHandle m_NodeReachedHandle;
	dae::ObserverHandle m_CollisionHandle;

	unsigned short m_ShotSoundId{};
};