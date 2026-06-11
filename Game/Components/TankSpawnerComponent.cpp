#include "TankSpawnerComponent.h"

#include "Minigin/GameObject.h"
#include "Minigin/Loading/LoadingHelpers.h"
#include "Minigin/Scene.h"
#include "Minigin/Sound/ServiceLocator.h"

#include <algorithm>
#include <utility>

TankSpawnerComponent::TankSpawnerComponent(dae::GameObject& owner)
	: Component(owner)
{
}

void TankSpawnerComponent::Start()
{
	const int playerCount = std::min(m_PlayerCount, static_cast<int>(m_PlayerSpawnNodes.size()));
	const int enemyCount = std::min(m_EnemyCount, static_cast<int>(m_EnemySpawnNodes.size()));

	for (int index = 0; index < playerCount; ++index)
	{
		SpawnPlayerTank(index, GetSpawnNode(m_PlayerSpawnNodes, index));
	}

	for (int index = 0; index < enemyCount; ++index)
	{
		SpawnEnemyTank(index, GetSpawnNode(m_EnemySpawnNodes, index));
	}
}

void TankSpawnerComponent::Initialize(std::vector<dae::GameObject*> playerSpawnNodes, int playerCount, std::vector<dae::GameObject*> enemySpawnNodes, int enemyCount)
{
	m_PlayerSpawnNodes = std::move(playerSpawnNodes);
	m_PlayerCount = playerCount;
	m_EnemySpawnNodes = std::move(enemySpawnNodes);
	m_EnemyCount = enemyCount;
}

dae::GameObject* TankSpawnerComponent::SpawnPlayerTank(int index, dae::GameObject* startNode)
{
	if (!startNode)
		return nullptr;

	dae::Scene* scene = GetOwner()->GetScene();
	if (!scene)
		return nullptr;

	dae::ParamMap overrides;
	overrides["device"] = GetPlayerDevice(index);
	overrides["startNode"] = startNode;

	return dae::ServiceLocator::GetPrefabManager().Instantiate(
		m_PlayerPrefab,
		*scene,
		startNode->GetTransform().GetWorldPosition(),
		overrides
	);
}

dae::GameObject* TankSpawnerComponent::SpawnEnemyTank(int, dae::GameObject* startNode)
{
	if (!startNode)
		return nullptr;

	dae::Scene* scene = GetOwner()->GetScene();
	if (!scene)
		return nullptr;

	dae::ParamMap overrides;
	overrides["startNode"] = startNode;

	return dae::ServiceLocator::GetPrefabManager().Instantiate(
		m_EnemyPrefab,
		*scene,
		startNode->GetTransform().GetWorldPosition(),
		overrides
	);
}

dae::GameObject* TankSpawnerComponent::GetSpawnNode(const std::vector<dae::GameObject*>& nodes, int index) const
{
	if (index < 0 || index >= static_cast<int>(nodes.size()))
		return nullptr;

	return nodes[index];
}

std::string TankSpawnerComponent::GetPlayerDevice(int index) const
{
	if (index >= 0 && index < static_cast<int>(m_PlayerDevices.size()))
		return m_PlayerDevices[index];

	return "";
}

std::vector<dae::ParamDefinition> TankSpawnerComponent::GetExpectedParams() const
{
	std::vector<dae::GameObject*> emptyNodes;
	std::vector<std::string> playerDevices;

	return
	{
		{ "playerPrefab", std::string("playerTank.prefab") },
		{ "playerSpawnNodes", emptyNodes },
		{ "playerCount", 1 },
		{ "playerDevices", playerDevices },
		{ "enemyPrefab", std::string("enemyTank.prefab") },
		{ "enemySpawnNodes", emptyNodes },
		{ "enemyCount", 0 }
	};
}

void TankSpawnerComponent::Load(const dae::ParamMap& params)
{
	const std::vector<dae::GameObject*> playerSpawnNodes =
		dae::GetRequiredParam<std::vector<dae::GameObject*>>(params, "playerSpawnNodes");
	const int playerCount = dae::GetRequiredParam<int>(params, "playerCount");
	const std::vector<dae::GameObject*> enemySpawnNodes =
		dae::GetRequiredParam<std::vector<dae::GameObject*>>(params, "enemySpawnNodes");
	const int enemyCount = dae::GetRequiredParam<int>(params, "enemyCount");

	Initialize(playerSpawnNodes, playerCount, enemySpawnNodes, enemyCount);

	m_PlayerPrefab = dae::GetOptionalParam<std::string>(params, "playerPrefab", m_PlayerPrefab);
	m_EnemyPrefab = dae::GetOptionalParam<std::string>(params, "enemyPrefab", m_EnemyPrefab);
	m_PlayerDevices = dae::GetOptionalParam<std::vector<std::string>>(params, "playerDevices", m_PlayerDevices);
}
