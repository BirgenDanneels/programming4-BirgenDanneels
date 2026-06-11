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

	dae::GameObject* GetSpawnNode(const std::vector<dae::GameObject*>& nodes, int index) const;
	std::string GetPlayerDevice(int index) const;

	std::string m_PlayerPrefab{ "playerTank.prefab" };
	std::string m_EnemyPrefab{ "enemyTank.prefab" };

	std::vector<dae::GameObject*> m_PlayerSpawnNodes;
	std::vector<dae::GameObject*> m_EnemySpawnNodes;
	std::vector<std::string> m_PlayerDevices{ "gamepad_0", "gamepad_1" };

	int m_PlayerCount{ 1 };
	int m_EnemyCount{ 0 };
};
