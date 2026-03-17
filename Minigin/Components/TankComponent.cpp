#include "TankComponent.h"
#include "CharacterController.h"
#include "HealthComponent.h"
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
}

dae::TankComponent::TankComponent(GameObject& pOwner)
	: Component(pOwner), m_pCharacterController(GetOwner()->GetComponent<CharacterController>()), m_pHealthComponent(GetOwner()->GetComponent<HealthComponent>())
{
	if (!m_pCharacterController)
		m_pCharacterController = GetOwner()->AddComponent<CharacterController>();

	if (!m_pHealthComponent)
		m_pHealthComponent = GetOwner()->AddComponent<HealthComponent>();
}

dae::TankComponent::~TankComponent() = default;

void dae::TankComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::TankComponent::Initialize(InputDevice* device, float speed, int lives)
{
	m_pCharacterController->SetSpeed(speed);
	m_pHealthComponent->Initialize(lives);

	m_pMoveCommand = std::make_unique<Move2DCommand>(*m_pCharacterController);
	m_pDamageCommand = std::make_unique<DamageCommand>(*m_pHealthComponent, 1);

	if (device)
	{
		//Bind the input map to the device
		auto inputMap = std::make_unique<InputMap>();

		if (dynamic_cast<Keyboard*>(device))
		{
			inputMap->BindAxis2D("Move", SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, *m_pMoveCommand);
			inputMap->BindAction("Damage", SDL_SCANCODE_C, InputState::Pressed, *m_pDamageCommand);
		}
		else
		{
			inputMap->BindAxis2D("Move", (int)GamepadInput::DPadLeft, (int)GamepadInput::DPadRight, (int)GamepadInput::DPadUp, (int)GamepadInput::DPadDown, *m_pMoveCommand);
			inputMap->BindAxis2D("Move", (int)GamepadInput::LeftStickLeft, (int)GamepadInput::LeftStickRight, (int)GamepadInput::LeftStickUp, (int)GamepadInput::LeftStickDown, *m_pMoveCommand);
			inputMap->BindAction("Damage", (int)GamepadInput::X, InputState::Pressed, *m_pDamageCommand);
		}


		device->SetInputMap(std::move(inputMap));
	}
}