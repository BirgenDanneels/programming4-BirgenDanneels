#include "TankComponent.h"
#include "HealthComponent.h"
#include "PointsComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/InputManager.h"
#include "Minigin/Input/InputMap.h"
#include "Game/Commands/MoveCommands.h"
#include "Minigin/Physics/Rigidbody.h"
#include "Minigin/Physics/Collider.h"
#include "BarrelComponent.h"
#include "Minigin/Loading/LoadingHelpers.h"

class DamageCommand final : public dae::Command
{
public:
	DamageCommand(TankComponent& tankComponent, int damage)
		: m_tankComponent(tankComponent), m_damage(damage)
	{
	}
	void Execute() override
	{
		m_tankComponent.TakeDamage(m_damage);
	}
private:
	TankComponent& m_tankComponent;
	int m_damage;
};

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

class FirePickupEventCommand final : public dae::Command
{
public:
	FirePickupEventCommand(TankComponent& tankComponent)
		: m_tankComponent(tankComponent)
	{
	}
	void Execute() override
	{
		m_tankComponent.RequestOrbPickUp();
	}
private:
	TankComponent& m_tankComponent;
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

TankComponent::TankComponent(dae::GameObject& pOwner)
	: Component(pOwner)
{
	GetOwner()->AddComponent<dae::Rigidbody>()->Initialize(false, true); //SHOULD BE REMOVED AND FIXED THAT OBJECTS CAN BE CONSTRUCTED DURING START bc now the vector gets cleared at the end of start which means the constructed objects get removed from said vector!
	
}

TankComponent::~TankComponent()
{
	if (m_pInputDevice)
	{
		dae::InputMap* inputMap = m_pInputDevice->GetInputMap("TankControls");
		if (inputMap)
		{
			inputMap->RemoveCommandFromAxis2DBinding("move", *m_pMoveCommand);
			inputMap->RemoveCommandFromAxisBinding("aim", *m_pRotateBarrelCommand);
			inputMap->RemoveCommandFromActionBinding("damage", *m_pDamageCommand);
			inputMap->RemoveCommandFromActionBinding("firePickUpEvent", *m_pPickupCommand);
			inputMap->RemoveCommandFromActionBinding("fire", *m_pShootCommand);
		}
	}
}

void TankComponent::Start()
{
	m_shotSound = dae::ServiceLocator::GetSoundSystem().GetSoundId("Shot.wav");

	m_pRigidbody = GetOwner()->GetComponent<dae::Rigidbody>();
	m_pHealthComponent = GetOwner()->GetComponent<HealthComponent>();
	m_pPointsComponent = GetOwner()->GetComponent<PointsComponent>();

	if (!m_pRigidbody)
	{
		m_pRigidbody = GetOwner()->AddComponent<dae::Rigidbody>();
		m_pRigidbody->Initialize(false, true);
	}

	m_pRigidbody->SetDrag(0.1f);

	if (!m_pHealthComponent)
		m_pHealthComponent = GetOwner()->AddComponent<HealthComponent>();

	if (!m_pPointsComponent)
		m_pPointsComponent = GetOwner()->AddComponent<PointsComponent>();



	m_pMoveCommand = std::make_unique<Move2DCommand>(*m_pRigidbody, m_Speed);
	m_pDamageCommand = std::make_unique<DamageCommand>(*this, 1);
	m_pPickupCommand = std::make_unique<FirePickupEventCommand>(*this);
	m_pShootCommand = std::make_unique<ShootCommand>(*m_pBarrel->GetComponent<BarrelComponent>());
	m_pRotateBarrelCommand = std::make_unique<RotateBarrelCommand>(*m_pBarrel->GetComponent<BarrelComponent>());

	//Bind add pointsystems to events
	m_pointObserverHandle = OnTankEvent().AddObserver(m_pPointsComponent);

	if (m_pInputDevice)
	{
		//Bind the input map to the device
		auto inputMap = m_pInputDevice->GetInputMap("TankControls");

		inputMap->AddCommandToActionBinding("damage", *m_pDamageCommand);
		inputMap->AddCommandToActionBinding("firePickUpEvent", *m_pPickupCommand);
		inputMap->AddCommandToActionBinding("fire", *m_pShootCommand);
		inputMap->AddCommandToAxis2DBinding("move", *m_pMoveCommand);
		inputMap->AddCommandToAxisBinding("aim", *m_pRotateBarrelCommand);

		//inputMap->BindAxis2D("Move", (int)GamepadInput::DPadLeft, (int)GamepadInput::DPadRight, (int)GamepadInput::DPadUp, (int)GamepadInput::DPadDown, *m_pMoveCommand);


		m_pInputDevice->SetActiveInputMap("TankControls");
	}

	//OnCollission
	dae::Collider* collider = GetOwner()->GetComponent<dae::Collider>();
	if (collider)
	{
		m_collisionHandle = collider->OnCollision().AddObserver(this);
	}
}

void TankComponent::Update(float)
{	
	glm::vec2 v = m_pRigidbody->GetVelocity();
	float angleDeg = glm::degrees(atan2(v.y, v.x));

	GetOwner()->GetTransform().SetLocalRotation(angleDeg);
}

void TankComponent::RequestEnemyKill()
{
	m_onTankEventSubject.NotifyObservers(TankEvents::KillEnemy);
	dae::ServiceLocator::GetSoundSystem().Play(m_shotSound, 0.2f);
}

void TankComponent::RequestOrbPickUp()
{
	m_onTankEventSubject.NotifyObservers(TankEvents::PickupOrb);
}

void TankComponent::TakeDamage(int damage)
{
	m_pHealthComponent->TakeDamage(damage);
}

void TankComponent::Initialize(const std::string& device, float speed, int lives, dae::GameObject& barrel)
{
	m_pHealthComponent = GetOwner()->GetComponent<HealthComponent>();
	m_pHealthComponent->Initialize(lives);
	m_Speed = speed;
	m_pInputDevice = dae::InputManager::GetInstance().GetDeviceByName(device);
	m_pBarrel = &barrel;
}

std::vector<dae::ParamDefinition> TankComponent::GetExpectedParams() const
{
	dae::GameObject* nullPtr{ nullptr };

	return { { "speed", 100.0f }, { "lives", 3 }, { "barrel", nullPtr }, { "device", "" } };
}

void TankComponent::Load(const dae::ParamMap& params)
{
	float speed = GetRequiredParam<float>(params, "speed");
	int lives = GetRequiredParam<int>(params, "lives");
	dae::GameObject* barrel = GetRequiredParam<dae::GameObject*>(params, "barrel");
	const std::string& device = GetRequiredParam<std::string>(params, "device");
	Initialize(device, speed, lives, *barrel);
}

void TankComponent::OnNotify(dae::HitEvent)
{
}
