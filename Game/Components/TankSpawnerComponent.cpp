#include "TankSpawnerComponent.h"

#include "BarrelComponent.h"
#include "EnemyTankComponent.h"
#include "HealthComponent.h"
#include "PointsComponent.h"
#include "TankComponent.h"

#include "Minigin/Components/TextureComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/Loading/LoadingHelpers.h"
#include "Minigin/Physics/Collider.h"
#include "Minigin/Scene.h"

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

	dae::GameObject* tank = scene->CreateGameObject();
	tank->SetTag("Player");
	tank->GetTransform().SetWorldPosition(startNode->GetTransform().GetWorldPosition());
	tank->AddComponent<dae::TextureComponent>()->Initialize("RedTank.png", true);
	tank->AddComponent<HealthComponent>()->Initialize(m_PlayerLives);
	tank->AddComponent<PointsComponent>();
	tank->AddComponent<dae::Collider>()->InitializeBoxCollider(m_ColliderWidth, m_ColliderHeight);

	dae::GameObject* barrel = scene->CreateGameObject();
	barrel->SetParent(tank, false);
	barrel->GetTransform().SetLocalPosition(0.0f, 0.0f);

	dae::GameObject* bulletSpawn = CreateBulletSpawnPoint(*barrel);
	barrel->AddComponent<BarrelComponent>()->Initialize(
		m_PlayerFireRate,
		*bulletSpawn,
		m_PlayerDamage,
		m_PlayerMaxBounces,
		m_PlayerBarrelRotationSpeed
	);

	tank->AddComponent<TankComponent>()->Initialize(
		GetPlayerDevice(index),
		m_PlayerSpeed,
		m_PlayerLives,
		*barrel,
		startNode
	);

	return tank;
}

dae::GameObject* TankSpawnerComponent::SpawnEnemyTank(int, dae::GameObject* startNode)
{
	if (!startNode)
		return nullptr;

	dae::Scene* scene = GetOwner()->GetScene();
	if (!scene)
		return nullptr;

	dae::GameObject* tank = scene->CreateGameObject();
	tank->SetTag("Enemy");
	tank->GetTransform().SetWorldPosition(startNode->GetTransform().GetWorldPosition());
	tank->AddComponent<dae::TextureComponent>()->Initialize("BlueTank.png", true);
	tank->AddComponent<HealthComponent>()->Initialize(m_EnemyLives);
	tank->AddComponent<dae::Collider>()->InitializeBoxCollider(m_ColliderWidth, m_ColliderHeight);

	dae::GameObject* bulletSpawn = CreateBulletSpawnPoint(*tank);
	tank->AddComponent<EnemyTankComponent>()->Initialize(
		m_EnemySpeed,
		m_EnemyLives,
		*bulletSpawn,
		startNode,
		m_NodeReachDistance,
		m_EnemyVisionLength,
		m_EnemyFireRate,
		m_EnemyDamage,
		m_EnemyMaxBounces
	);

	return tank;
}

// Will be replaced by prefabs
dae::GameObject* TankSpawnerComponent::CreateBulletSpawnPoint(dae::GameObject& parent)
{
	dae::Scene* scene = GetOwner()->GetScene();
	if (!scene)
		return nullptr;

	dae::GameObject* bulletSpawn = scene->CreateGameObject();
	bulletSpawn->SetParent(&parent, false);
	bulletSpawn->GetTransform().SetLocalPosition(25.0f, 0.0f);
	return bulletSpawn;
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
		{ "playerSpawnNodes", emptyNodes },
		{ "playerCount", 1 },
		{ "playerDevices", playerDevices },
		{ "enemySpawnNodes", emptyNodes },
		{ "enemyCount", 0 },
		{ "playerSpeed", 200.0f },
		{ "playerLives", 3 },
		{ "playerFireRate", 0.5f },
		{ "playerDamage", 1 },
		{ "playerMaxBounces", 5 },
		{ "playerBarrelRotationSpeed", 180.0f },
		{ "enemySpeed", 120.0f },
		{ "enemyLives", 1 },
		{ "enemyVisionLength", 1000.0f },
		{ "enemyFireRate", 1.25f },
		{ "enemyDamage", 1 },
		{ "enemyMaxBounces", 0 },
		{ "nodeReachDistance", 1.0f },
		{ "colliderWidth", 25.0f },
		{ "colliderHeight", 25.0f }
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

	m_PlayerDevices = dae::GetOptionalParam<std::vector<std::string>>(params, "playerDevices", m_PlayerDevices);

	m_PlayerSpeed = dae::GetOptionalParam<float>(params, "playerSpeed", m_PlayerSpeed);
	m_PlayerLives = dae::GetOptionalParam<int>(params, "playerLives", m_PlayerLives);
	m_PlayerFireRate = dae::GetOptionalParam<float>(params, "playerFireRate", m_PlayerFireRate);
	m_PlayerDamage = dae::GetOptionalParam<int>(params, "playerDamage", m_PlayerDamage);
	m_PlayerMaxBounces = dae::GetOptionalParam<int>(params, "playerMaxBounces", m_PlayerMaxBounces);
	m_PlayerBarrelRotationSpeed = dae::GetOptionalParam<float>(params, "playerBarrelRotationSpeed", m_PlayerBarrelRotationSpeed);

	m_EnemySpeed = dae::GetOptionalParam<float>(params, "enemySpeed", m_EnemySpeed);
	m_EnemyLives = dae::GetOptionalParam<int>(params, "enemyLives", m_EnemyLives);
	m_EnemyVisionLength = dae::GetOptionalParam<float>(params, "enemyVisionLength", m_EnemyVisionLength);
	m_EnemyFireRate = dae::GetOptionalParam<float>(params, "enemyFireRate", m_EnemyFireRate);
	m_EnemyDamage = dae::GetOptionalParam<int>(params, "enemyDamage", m_EnemyDamage);
	m_EnemyMaxBounces = dae::GetOptionalParam<int>(params, "enemyMaxBounces", m_EnemyMaxBounces);

	m_NodeReachDistance = dae::GetOptionalParam<float>(params, "nodeReachDistance", m_NodeReachDistance);
	m_ColliderWidth = dae::GetOptionalParam<float>(params, "colliderWidth", m_ColliderWidth);
	m_ColliderHeight = dae::GetOptionalParam<float>(params, "colliderHeight", m_ColliderHeight);
}
