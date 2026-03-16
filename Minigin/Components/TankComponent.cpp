#include "TankComponent.h"
#include "CharacterController.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Input/InputMap.h"
#include "Commands/MoveCommands.h"

dae::TankComponent::TankComponent(GameObject& pOwner)
	: Component(pOwner), m_pCharacterController(GetOwner()->GetComponent<CharacterController>())
{
	if (!m_pCharacterController)
		m_pCharacterController = GetOwner()->AddComponent<CharacterController>();
}

dae::TankComponent::~TankComponent() = default;

void dae::TankComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::TankComponent::Initialize(InputDevice* device, float speed)
{
	m_pCharacterController->SetSpeed(speed);

	m_pMoveCommand = std::make_unique<Move2DCommand>(*m_pCharacterController);

	if (device)
	{
		//Bind the input map to the device
		auto inputMap = std::make_unique<InputMap>();

		if (dynamic_cast<Keyboard*>(device))
		{
			inputMap->BindAxis2D("Move", SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, *m_pMoveCommand.get());

		}
		else
		{
			inputMap->BindAxis2D("Move", (int)GamepadInput::DPadLeft, (int)GamepadInput::DPadRight, (int)GamepadInput::DPadUp, (int)GamepadInput::DPadDown, *m_pMoveCommand.get());
			inputMap->BindAxis2D("Move", (int)GamepadInput::LeftStickLeft, (int)GamepadInput::LeftStickRight, (int)GamepadInput::LeftStickUp, (int)GamepadInput::LeftStickDown, *m_pMoveCommand.get());
		}


		device->SetInputMap(std::move(inputMap));
	}
}
