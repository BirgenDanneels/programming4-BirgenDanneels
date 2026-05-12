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
	m_shotSound = dae::ServiceLocator::GetSoundSystem().LoadSound("Shot.wav");


	GetOwner()->AddComponent<dae::Rigidbody>()->Initialize(false, true); //SHOULD BE REMOVED AND FIXED THAT OBJECTS CAN BE CONSTRUCTED DURING START bc now the vector gets cleared at the end of start which means the constructed objects get removed from said vector!
	
}

TankComponent::~TankComponent()
{
	if (m_pInputDevice)
	{
		dae::InputMap* inputMap = m_pInputDevice->GetInputMap();
		if (inputMap)
		{
			inputMap->UnbindAxis2D("Move");
			inputMap->UnbindAction("Damage");
			inputMap->UnbindAction("FirePickUpEvent");
			inputMap->UnbindAction("FireKillEvent");
		}
	}
}

void TankComponent::Start()
{
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
		auto inputMap = std::make_unique<dae::InputMap>();

		if (dynamic_cast<dae::Keyboard*>(m_pInputDevice))
		{
			using namespace dae;

			inputMap->BindAxis2D("Move", SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, *m_pMoveCommand);
			inputMap->BindAxis("RotateBarrel", SDL_SCANCODE_B, SDL_SCANCODE_N, *m_pRotateBarrelCommand);
			inputMap->BindAction("Damage", SDL_SCANCODE_C, InputState::Pressed, *m_pDamageCommand);
			inputMap->BindAction("FirePickUpEvent", SDL_SCANCODE_Z, InputState::Pressed, *m_pPickupCommand);
			inputMap->BindAction("FireKillEvent", SDL_SCANCODE_X, InputState::Pressed, *m_pShootCommand);
		}
		else
		{
			using namespace dae;

			inputMap->BindAxis2D("Move", (int)GamepadInput::DPadLeft, (int)GamepadInput::DPadRight, (int)GamepadInput::DPadUp, (int)GamepadInput::DPadDown, *m_pMoveCommand);
			inputMap->BindAxis2D("Move", (int)GamepadInput::LeftStickLeft, (int)GamepadInput::LeftStickRight, (int)GamepadInput::LeftStickUp, (int)GamepadInput::LeftStickDown, *m_pMoveCommand);
			inputMap->BindAxis("RotateBarrel", (int)GamepadInput::LeftShoulder, (int)GamepadInput::RightShoulder, *m_pRotateBarrelCommand);
			inputMap->BindAction("Damage", (int)GamepadInput::X, InputState::Pressed, *m_pDamageCommand);
			inputMap->BindAction("FirePickUpEvent", (int)GamepadInput::A, InputState::Pressed, *m_pPickupCommand);
			inputMap->BindAction("FireKillEvent", (int)GamepadInput::B, InputState::Pressed, *m_pShootCommand);


		}


		m_pInputDevice->SetInputMap(std::move(inputMap));
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

void TankComponent::Initialize(dae::InputDevice* device, float speed, int lives, dae::GameObject& barrel)
{
	m_pHealthComponent = GetOwner()->GetComponent<HealthComponent>();
	m_pHealthComponent->Initialize(lives);
	m_Speed = speed;
	m_pInputDevice = device;
	m_pBarrel = &barrel;
}

void TankComponent::OnNotify(dae::HitEvent /*hit*/)
{
}
