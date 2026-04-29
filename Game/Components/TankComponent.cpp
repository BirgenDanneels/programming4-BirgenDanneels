#include "TankComponent.h"
#include "Minigin/Components/CharacterController.h"
#include "HealthComponent.h"
#include "PointsComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/InputManager.h"
#include "Minigin/Input/InputMap.h"
#include "Game/Commands/MoveCommands.h"

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
	: Component(pOwner), m_pCharacterController(GetOwner()->GetComponent<dae::CharacterController>()), m_pHealthComponent(GetOwner()->GetComponent<dae::HealthComponent>()), m_pPointsComponent(GetOwner()->GetComponent<dae::PointsComponent>())
{
	if (!m_pCharacterController)
		m_pCharacterController = GetOwner()->AddComponent<dae::CharacterController>();

	if (!m_pHealthComponent)
		m_pHealthComponent = GetOwner()->AddComponent<dae::HealthComponent>();

	if (!m_pPointsComponent)
		m_pPointsComponent = GetOwner()->AddComponent<dae::PointsComponent>();

	m_shotSound = dae::ServiceLocator::GetSoundSystem().LoadSound("Shot.wav");

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

void TankComponent::Update(float deltaTime)
{
	(void)deltaTime;
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
	m_pCharacterController->SetSpeed(speed);
	m_pHealthComponent->Initialize(lives);

	//Bind add pointsystems to events
	OnTankEvent().AddObserver(m_pPointsComponent);

	m_pMoveCommand = std::make_unique<dae::Move2DCommand>(*m_pCharacterController);
	m_pDamageCommand = std::make_unique<DamageCommand>(*m_pHealthComponent, 1);
	m_pPickupCommand = std::make_unique<FirePickupEventCommand>(*this);
	m_pKillCommand = std::make_unique<FireKillEventCommand>(*this);

	m_pInputDevice = device;

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
}
