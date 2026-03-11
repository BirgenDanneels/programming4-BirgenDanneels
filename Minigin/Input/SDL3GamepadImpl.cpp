#include "SDL3GamepadImpl.h"
#include "GamepadInput.h"
#include "Bindings/InputStates.h"
#include <cmath>

dae::SDL3GamepadImpl::SDL3GamepadImpl(int controllerIdx)
	: m_controllerIdx(controllerIdx)
{
	// Initialize SDL gamepad subsystem (if not already)
	if (!SDL_WasInit(SDL_INIT_GAMEPAD))
	{
		SDL_InitSubSystem(SDL_INIT_GAMEPAD);
	}

	// Get available gamepads
	int numGamepads = 0;
	SDL_JoystickID* gamepads = SDL_GetGamepads(&numGamepads);
	
	if (gamepads && m_controllerIdx < numGamepads)
	{
		m_pGamepad = SDL_OpenGamepad(gamepads[m_controllerIdx]);
	}
	
	if (gamepads)
	{
		SDL_free(gamepads);
	}
}

dae::SDL3GamepadImpl::~SDL3GamepadImpl()
{
	if (m_pGamepad)
	{
		SDL_CloseGamepad(m_pGamepad);
		m_pGamepad = nullptr;
	}
}

void dae::SDL3GamepadImpl::Update()
{
	m_previousState = m_currentState;

	if (!m_pGamepad)
		return;

	// Update button states
	for (int i = 0; i < static_cast<int>(SDL_GAMEPAD_BUTTON_COUNT); ++i)
	{
		m_currentState.buttons[i] = SDL_GetGamepadButton(m_pGamepad, static_cast<SDL_GamepadButton>(i));
	}

	// Update axis states (normalize to -1.0 to 1.0)
	for (int i = 0; i < static_cast<int>(SDL_GAMEPAD_AXIS_COUNT); ++i)
	{
		Sint16 axisValue = SDL_GetGamepadAxis(m_pGamepad, static_cast<SDL_GamepadAxis>(i));
		m_currentState.axes[i] = static_cast<float>(axisValue) / 32767.0f;
	}
}

SDL_GamepadButton dae::SDL3GamepadImpl::ToSDLButton(int key)
{
	switch (static_cast<GamepadInput>(key))
	{
	case GamepadInput::DPadUp: return SDL_GAMEPAD_BUTTON_DPAD_UP;
	case GamepadInput::DPadDown: return SDL_GAMEPAD_BUTTON_DPAD_DOWN;
	case GamepadInput::DPadLeft: return SDL_GAMEPAD_BUTTON_DPAD_LEFT;
	case GamepadInput::DPadRight: return SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
	case GamepadInput::Start: return SDL_GAMEPAD_BUTTON_START;
	case GamepadInput::Back: return SDL_GAMEPAD_BUTTON_BACK;
	case GamepadInput::LeftThumb: return SDL_GAMEPAD_BUTTON_LEFT_STICK;
	case GamepadInput::RightThumb: return SDL_GAMEPAD_BUTTON_RIGHT_STICK;
	case GamepadInput::LeftShoulder: return SDL_GAMEPAD_BUTTON_LEFT_SHOULDER;
	case GamepadInput::RightShoulder: return SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER;
	case GamepadInput::A: return SDL_GAMEPAD_BUTTON_SOUTH;
	case GamepadInput::B: return SDL_GAMEPAD_BUTTON_EAST;
	case GamepadInput::X: return SDL_GAMEPAD_BUTTON_WEST;
	case GamepadInput::Y: return SDL_GAMEPAD_BUTTON_NORTH;
	default: return SDL_GAMEPAD_BUTTON_INVALID;
	}
}

bool dae::SDL3GamepadImpl::GetKeyState(int key, InputState state) const
{
	if (!m_pGamepad)
		return false;

	bool currentPressed = false;
	bool previousPressed = false;

	if (IsAnalogInput(key))
	{
		currentPressed = GetAnalogButtonState(key, false);
		previousPressed = GetAnalogButtonState(key, true);
	}
	else
	{
		SDL_GamepadButton sdlButton = ToSDLButton(key);
		if (sdlButton != SDL_GAMEPAD_BUTTON_INVALID)
		{
			currentPressed = m_currentState.buttons[static_cast<int>(sdlButton)] != 0;
			previousPressed = m_previousState.buttons[static_cast<int>(sdlButton)] != 0;
		}
	}

	switch (state)
	{
	case InputState::Pressed:
		return currentPressed && !previousPressed;
	case InputState::Released:
		return !currentPressed && previousPressed;
	case InputState::Down:
		return currentPressed;
	default:
		return false;
	}
}

float dae::SDL3GamepadImpl::GetRawAxisValue(int key, const GamepadState& state) const
{
	float value = 0.0f;

	switch (static_cast<GamepadInput>(key))
	{
	case GamepadInput::LeftTrigger:

		// Triggers in SDL3 range from -1 to 1, but we want 0 to 1
		value = (state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFT_TRIGGER)] + 1.0f) * 0.5f;
		break;
	case GamepadInput::RightTrigger:
		value = (state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)] + 1.0f) * 0.5f;
		break;

	case GamepadInput::LeftStickLeft:
		value = state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTX)];
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::LeftStickRight:
		value = state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTX)];
		value = value > 0.0f ? value : 0.0f;
		break;
	case GamepadInput::LeftStickUp:

		// SDL Y axis: negative is up
		value = state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTY)];
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::LeftStickDown:
		value = state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTY)];
		value = value > 0.0f ? value : 0.0f;
		break;

	case GamepadInput::RightStickLeft:
		value = state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTX)];
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::RightStickRight:
		value = state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTX)];
		value = value > 0.0f ? value : 0.0f;
		break;
	case GamepadInput::RightStickUp:
		value = state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTY)];
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::RightStickDown:
		value = state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTY)];
		value = value > 0.0f ? value : 0.0f;
		break;

	default:
		SDL_GamepadButton sdlButton = ToSDLButton(key);
		if (sdlButton != SDL_GAMEPAD_BUTTON_INVALID)
		{
			return state.buttons[static_cast<int>(sdlButton)] ? 1.0f : 0.0f;
		}
		break;
	}

	return value;
}

float dae::SDL3GamepadImpl::GetAxisValue(int key) const
{
	if (!m_pGamepad)
		return 0.0f;

	float value = GetRawAxisValue(key, m_currentState);

	// Apply deadzone
	if (std::abs(value) < m_deadzone)
		return 0.0f;

	return value;
}

bool dae::SDL3GamepadImpl::IsAnalogInput(int key) const
{
	return key > static_cast<int>(GamepadInput::Y);
}

bool dae::SDL3GamepadImpl::GetAnalogButtonState(int key, bool usePrevious) const
{
	const GamepadState& state = usePrevious ? m_previousState : m_currentState;
	float value = GetRawAxisValue(key, state);

	// Apply deadzone before threshold comparison
	if (std::abs(value) < m_deadzone)
		return false;

	// Compare against threshold
	return value >= m_analogToButtonThreshold;
}