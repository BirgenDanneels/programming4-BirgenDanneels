#include "TankComponent.h"
#include "HealthComponent.h"
#include "PointsComponent.h"
#include "NodeMovementComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/InputManager.h"
#include "Minigin/Input/InputMap.h"
#include "Minigin/Physics/Collider.h"
#include "BarrelComponent.h"
#include "Minigin/Loading/LoadingHelpers.h"

class ShootCommand final : public dae::Command
{
public:
	ShootCommand(BarrelComponent& barrelComponent)
		: m_barrelComponent(barrelComponent)
	{
	}
	void Execute() override
	{
		m_barrelComponent.Shoot();
	}
private:
	BarrelComponent& m_barrelComponent;
};

class RotateBarrelCommand final : public dae::Axis1DCommand
{
public:
	RotateBarrelCommand(BarrelComponent& barrelComponent)
		: m_barrelComponent(barrelComponent)
	{
	}
	void Execute() override
	{
		m_barrelComponent.Rotate(GetAxisValue());
	}
private:
	BarrelComponent& m_barrelComponent;
};

class TankMoveCommand final : public dae::Axis2DCommand
{
public:
	TankMoveCommand(TankComponent& tankComponent)
		: m_tankComponent(tankComponent)
	{
	}

	void Execute() override
	{
		m_tankComponent.SetHeldMoveInput(GetAxisValue());
	}

private:
	TankComponent& m_tankComponent;
};

TankComponent::TankComponent(dae::GameObject& owner)
	: Component(owner)
{
	// TODO: Remove this
	if (!GetOwner()->GetComponent<NodeMovementComponent>())
	{
		GetOwner()->AddComponent<NodeMovementComponent>();
	}
}

TankComponent::~TankComponent()
{
	if (!m_pInputDevice)
		return;

	dae::InputMap* inputMap = m_pInputDevice->GetInputMap("TankControls");
	if (!inputMap)
		return;

	inputMap->RemoveCommandFromAxis2DBinding("move", *m_pMoveCommand);
	inputMap->RemoveCommandFromAxisBinding("aim", *m_pRotateBarrelCommand);
	inputMap->RemoveCommandFromActionBinding("fire", *m_pShootCommand);
}

void TankComponent::Start()
{
	m_shotSound = dae::ServiceLocator::GetSoundSystem().GetSoundId("Shot.wav");

	m_pHealthComponent = GetOwner()->GetComponent<HealthComponent>();
	m_pPointsComponent = GetOwner()->GetComponent<PointsComponent>();
	m_pNodeMovementComponent = GetOwner()->GetComponent<NodeMovementComponent>();

	if (!m_pHealthComponent)
		m_pHealthComponent = GetOwner()->AddComponent<HealthComponent>();

	if (!m_pPointsComponent)
		m_pPointsComponent = GetOwner()->AddComponent<PointsComponent>();

	if (!m_pNodeMovementComponent)
		m_pNodeMovementComponent = GetOwner()->AddComponent<NodeMovementComponent>();

	m_pHealthComponent->Initialize(m_Lives);
	m_pNodeMovementComponent->Initialize(m_Speed, m_pStartNode, m_NodeReachDistance);

	BarrelComponent* barrelComponent = m_pBarrel->GetComponent<BarrelComponent>();
	if (!barrelComponent)
		return;

	m_pMoveCommand = std::make_unique<TankMoveCommand>(*this);
	m_pShootCommand = std::make_unique<ShootCommand>(*barrelComponent);
	m_pRotateBarrelCommand = std::make_unique<RotateBarrelCommand>(*barrelComponent);

	m_pointObserverHandle = OnTankEvent().AddObserver(m_pPointsComponent);

	if (m_pInputDevice)
	{
		dae::InputMap* inputMap = m_pInputDevice->GetInputMap("TankControls");

		if (inputMap)
		{
			// TODO: Remove binds from input map
			// inputMap->AddCommandToActionBinding("damage", *m_pDamageCommand);
			// inputMap->AddCommandToActionBinding("firePickUpEvent", *m_pPickupCommand);
			inputMap->AddCommandToActionBinding("fire", *m_pShootCommand);
			inputMap->AddCommandToAxis2DBinding("move", *m_pMoveCommand);
			inputMap->AddCommandToAxisBinding("aim", *m_pRotateBarrelCommand);
		}

		m_pInputDevice->SetActiveInputMap("TankControls");
	}

	//OnCollission
	dae::Collider* collider = GetOwner()->GetComponent<dae::Collider>();
	if (collider)
	{
		m_collisionHandle = collider->OnCollision().AddObserver(this);
	}
}

void TankComponent::SetHeldMoveInput(const glm::vec3& input)
{
	if (m_pNodeMovementComponent)
	{
		m_pNodeMovementComponent->SetHeldMoveInput(input);
	}
}

void TankComponent::TakeDamage(int damage)
{
	if (m_pHealthComponent)
	{
		m_pHealthComponent->TakeDamage(damage);
	}
}

void TankComponent::Initialize(const std::string& device, float speed, int lives, dae::GameObject& barrel, dae::GameObject* startNode)
{
	m_Speed = speed;
	m_Lives = lives;
	m_pBarrel = &barrel;
	m_pStartNode = startNode;

	m_pInputDevice = dae::InputManager::GetInstance().GetDeviceByName(device);

	if (m_pHealthComponent)
	{
		m_pHealthComponent->Initialize(lives);
	}

	if (m_pNodeMovementComponent)
	{
		m_pNodeMovementComponent->Initialize(m_Speed, m_pStartNode, m_NodeReachDistance);
	}
}

std::vector<dae::ParamDefinition> TankComponent::GetExpectedParams() const
{
	dae::GameObject* nullPtr{ nullptr };

	return
	{
		{ "speed", 100.0f },
		{ "lives", 3 },
		{ "barrel", nullPtr },
		{ "device", "" },
		{ "startNode", nullPtr },
		{ "nodeReachDistance", 1.0f }
	};
}

void TankComponent::Load(const dae::ParamMap& params)
{
	float speed = GetRequiredParam<float>(params, "speed");
	int lives = GetRequiredParam<int>(params, "lives");
	dae::GameObject* barrel = GetRequiredParam<dae::GameObject*>(params, "barrel");
	const std::string& device = GetRequiredParam<std::string>(params, "device");
	dae::GameObject* startNode = GetRequiredParam<dae::GameObject*>(params, "startNode");
	m_NodeReachDistance = GetRequiredParam<float>(params, "nodeReachDistance");

	Initialize(device, speed, lives, *barrel, startNode);
}

void TankComponent::OnNotify(dae::HitEvent)
{
}
