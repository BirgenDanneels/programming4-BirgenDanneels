#pragma once

#include "Minigin/Components/Component.h"

#include <string>
#include <vector>

namespace dae
{
	class GameObject;
}

class TankSpawnerComponent final : public dae::Component
{
public:
	explicit TankSpawnerComponent(dae::GameObject& owner);

	void Start() override;
	void FixedUpdate(float) override {}
	void Update(float) override {}
	void Render() const override {}

	void Initialize(std::vector<dae::GameObject*> playerSpawnNodes, int playerCount, std::vector<dae::GameObject*> enemySpawnNodes, int enemyCount);

	std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	void Load(const dae::ParamMap& params) override;

private:
	dae::GameObject* SpawnPlayerTank(int index, dae::GameObject* startNode);
	dae::GameObject* SpawnEnemyTank(int index, dae::GameObject* startNode);
	dae::GameObject* CreateBulletSpawnPoint(dae::GameObject& parent);

	dae::GameObject* GetSpawnNode(const std::vector<dae::GameObject*>& nodes, int index) const;
	std::string GetPlayerDevice(int index) const;

	std::vector<dae::GameObject*> m_PlayerSpawnNodes;
	std::vector<dae::GameObject*> m_EnemySpawnNodes;
	std::vector<std::string> m_PlayerDevices{ "gamepad_0", "gamepad_1" };

	int m_PlayerCount{ 1 };
	int m_EnemyCount{ 0 };

	float m_PlayerSpeed{ 200.0f };
	int m_PlayerLives{ 3 };
	float m_PlayerFireRate{ 0.5f };
	int m_PlayerDamage{ 1 };
	int m_PlayerMaxBounces{ 5 };
	float m_PlayerBarrelRotationSpeed{ 180.0f };

	float m_EnemySpeed{ 120.0f };
	int m_EnemyLives{ 1 };
	float m_EnemyVisionLength{ 1000.0f };
	float m_EnemyFireRate{ 1.25f };
	int m_EnemyDamage{ 1 };
	int m_EnemyMaxBounces{ 0 };

	float m_NodeReachDistance{ 1.0f };
	float m_ColliderWidth{ 25.0f };
	float m_ColliderHeight{ 25.0f };
};
