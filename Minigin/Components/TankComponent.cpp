#include "TankComponent.h"
#include "CharacterController.h"
#include "HealthComponent.h"
#include "PointsComponent.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Input/InputMap.h"
#include "Commands/MoveCommands.h"

namespace dae
{
	class DamageCommand final : public Command
	{
	public:
		DamageCommand(HealthComponent& healthComponent, int damage)
			: m_healthComponent(healthComponent), m_damage(damage)
		{
		}
		void Execute() override
		{
			m_healthComponent.TakeDamage(m_damage);
		}
	private:
		HealthComponent& m_healthComponent;
		int m_damage;
	};

	class FireKillEventCommand final : public Command
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

	class FirePickupEventCommand final : public Command
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


}

dae::TankComponent::TankComponent(GameObject& pOwner)
	: Component(pOwner), m_pCharacterController(GetOwner()->GetComponent<CharacterController>()), m_pHealthComponent(GetOwner()->GetComponent<HealthComponent>()), m_pPointsComponent(GetOwner()->GetComponent<PointsComponent>())
{
	if (!m_pCharacterController)
		m_pCharacterController = GetOwner()->AddComponent<CharacterController>();

	if (!m_pHealthComponent)
		m_pHealthComponent = GetOwner()->AddComponent<HealthComponent>();

	if (!m_pPointsComponent)
		m_pPointsComponent = GetOwner()->AddComponent<PointsComponent>();
}

dae::TankComponent::~TankComponent()
{
	if (m_pInputDevice)
	{
		InputMap* inputMap = m_pInputDevice->GetInputMap();
		if (inputMap)
		{
			inputMap->UnbindAxis2D("Move");
			inputMap->UnbindAction("Damage");
			inputMap->UnbindAction("FirePickUpEvent");
			inputMap->UnbindAction("FireKillEvent");
		}
	}
}

void dae::TankComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::TankComponent::RequestEnemyKill()
{
	m_onTankEventSubject.NotifyObservers(TankEvents::KillEnemy);
}

void dae::TankComponent::RequestOrbPickUp()
{
	m_onTankEventSubject.NotifyObservers(TankEvents::PickupOrb);
}

void dae::TankComponent::Initialize(InputDevice* device, float speed, int lives)
{
	m_pCharacterController->SetSpeed(speed);
	m_pHealthComponent->Initialize(lives);

	//Bind add pointsystems to events
	OnTankEvent().AddObserver(m_pPointsComponent);

	m_pMoveCommand = std::make_unique<Move2DCommand>(*m_pCharacterController);
	m_pDamageCommand = std::make_unique<DamageCommand>(*m_pHealthComponent, 1);
	m_pPickupCommand = std::make_unique<FirePickupEventCommand>(*this);
	m_pKillCommand = std::make_unique<FireKillEventCommand>(*this);

	m_pInputDevice = device;

	if (m_pInputDevice)
	{
		//Bind the input map to the device
		auto inputMap = std::make_unique<InputMap>();

		if (dynamic_cast<Keyboard*>(m_pInputDevice))
		{
			inputMap->BindAxis2D("Move", SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, *m_pMoveCommand);
			inputMap->BindAction("Damage", SDL_SCANCODE_C, InputState::Pressed, *m_pDamageCommand);
			inputMap->BindAction("FirePickUpEvent", SDL_SCANCODE_Z, InputState::Pressed, *m_pPickupCommand);
			inputMap->BindAction("FireKillEvent", SDL_SCANCODE_X, InputState::Pressed, *m_pKillCommand);
		}
		else
		{
			inputMap->BindAxis2D("Move", (int)GamepadInput::DPadLeft, (int)GamepadInput::DPadRight, (int)GamepadInput::DPadUp, (int)GamepadInput::DPadDown, *m_pMoveCommand);
			inputMap->BindAxis2D("Move", (int)GamepadInput::LeftStickLeft, (int)GamepadInput::LeftStickRight, (int)GamepadInput::LeftStickUp, (int)GamepadInput::LeftStickDown, *m_pMoveCommand);
			inputMap->BindAction("Damage", (int)GamepadInput::X, InputState::Pressed, *m_pDamageCommand);
			inputMap->BindAction("FirePickUpEvent", (int)GamepadInput::A, InputState::Pressed, *m_pPickupCommand);
			inputMap->BindAction("FireKillEvent", (int)GamepadInput::B, InputState::Pressed, *m_pKillCommand);
		}


		m_pInputDevice->SetInputMap(std::move(inputMap));
	}
}
