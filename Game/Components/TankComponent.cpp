#include "TankComponent.h"
#include "HealthComponent.h"
#include "PointsComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/InputManager.h"
#include "Minigin/Input/InputMap.h"
#include "Game/Commands/MoveCommands.h"
#include "Minigin/Physics/Rigidbody.h"
#include "Minigin/Physics/Collider.h"

class DamageCommand final : public dae::Command
{
public:
	DamageCommand(dae::HealthComponent& healthComponent, int damage)
		: m_healthComponent(healthComponent), m_damage(damage)
	{
	}
	void Execute() override
	{
		m_healthComponent.TakeDamage(m_damage);
	}
private:
	dae::HealthComponent& m_healthComponent;
	int m_damage;
};

class FireKillEventCommand final : public dae::Command
{
public:
	FireKillEventCommand(TankComponent& tankComponent)
		: m_tankComponent(tankComponent)
	{
	}
	void Execute() override
	{
		m_tankComponent.RequestEnemyKill();
	}
private:
	TankComponent& m_tankComponent;
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
	m_pHealthComponent = GetOwner()->GetComponent<dae::HealthComponent>();
	m_pPointsComponent = GetOwner()->GetComponent<dae::PointsComponent>();

	if (!m_pRigidbody)
	{
		m_pRigidbody = GetOwner()->AddComponent<dae::Rigidbody>();
		m_pRigidbody->Initialize(false, true);
	}

	m_pRigidbody->SetDrag(0.1f);

	if (!m_pHealthComponent)
		m_pHealthComponent = GetOwner()->AddComponent<dae::HealthComponent>();

	if (!m_pPointsComponent)
		m_pPointsComponent = GetOwner()->AddComponent<dae::PointsComponent>();



	m_pMoveCommand = std::make_unique<dae::Move2DCommand>(*m_pRigidbody, m_Speed);
	m_pDamageCommand = std::make_unique<DamageCommand>(*m_pHealthComponent, 1);
	m_pPickupCommand = std::make_unique<FirePickupEventCommand>(*this);
	m_pKillCommand = std::make_unique<FireKillEventCommand>(*this);

	//Bind add pointsystems to events
	OnTankEvent().AddObserver(m_pPointsComponent);

	if (m_pInputDevice)
	{
		//Bind the input map to the device
		auto inputMap = std::make_unique<dae::InputMap>();

		if (dynamic_cast<dae::Keyboard*>(m_pInputDevice))
		{
			using namespace dae;

			inputMap->BindAxis2D("Move", SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, *m_pMoveCommand);
			inputMap->BindAction("Damage", SDL_SCANCODE_C, InputState::Pressed, *m_pDamageCommand);
			inputMap->BindAction("FirePickUpEvent", SDL_SCANCODE_Z, InputState::Pressed, *m_pPickupCommand);
			inputMap->BindAction("FireKillEvent", SDL_SCANCODE_X, InputState::Pressed, *m_pKillCommand);
		}
		else
		{
			using namespace dae;

			inputMap->BindAxis2D("Move", (int)GamepadInput::DPadLeft, (int)GamepadInput::DPadRight, (int)GamepadInput::DPadUp, (int)GamepadInput::DPadDown, *m_pMoveCommand);
			inputMap->BindAxis2D("Move", (int)GamepadInput::LeftStickLeft, (int)GamepadInput::LeftStickRight, (int)GamepadInput::LeftStickUp, (int)GamepadInput::LeftStickDown, *m_pMoveCommand);
			inputMap->BindAction("Damage", (int)GamepadInput::X, InputState::Pressed, *m_pDamageCommand);
			inputMap->BindAction("FirePickUpEvent", (int)GamepadInput::A, InputState::Pressed, *m_pPickupCommand);
			inputMap->BindAction("FireKillEvent", (int)GamepadInput::B, InputState::Pressed, *m_pKillCommand);


		}


		m_pInputDevice->SetInputMap(std::move(inputMap));
	}

	//OnCollission
	dae::Collider* collider = GetOwner()->GetComponent<dae::Collider>();
	if (collider)
	{
		collider->OnCollisionEnter().AddObserver(this);
	}
}

void TankComponent::Update(float deltaTime)
{
	(void)deltaTime;
	
	//X axis is straight ahead

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

void TankComponent::Initialize(dae::InputDevice* device, float speed, int lives)
{
	m_pHealthComponent = GetOwner()->GetComponent<dae::HealthComponent>();
	m_pHealthComponent->Initialize(lives);
	m_Speed = speed;
	m_pInputDevice = device;
}

void TankComponent::OnNotify(dae::Hit hit)
{
	(void)hit;
}
