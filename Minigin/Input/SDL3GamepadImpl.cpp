#include "GamepadImpl.h"
#include "GamepadInput.h"
#include "Bindings/InputStates.h"
#include <SDL3/SDL.h>
#include <cmath>

namespace dae
{
	class SDL3GamepadImpl : public GamepadImpl
	{
	public:
		explicit SDL3GamepadImpl(int controllerIdx);
		~SDL3GamepadImpl() override;

		void Update() override;
		bool GetKeyState(int key, InputState state) const override;
		float GetAxisValue(int key) const override;
		bool IsConnected() const override { return m_pGamepad != nullptr; }

		void SetDeadzone(float deadzone) override { m_deadzone = deadzone; }
		void SetAnalogToButtonThreshold(float threshold) override { m_analogToButtonThreshold = threshold; }

	private:
		SDL_Gamepad* m_pGamepad{ nullptr };
		int m_controllerIdx;

		struct GamepadState
		{
			Uint8 buttons[static_cast<int>(SDL_GAMEPAD_BUTTON_COUNT)]{};
			float axes[static_cast<int>(SDL_GAMEPAD_AXIS_COUNT)]{};
		};

		GamepadState m_currentState{};
		GamepadState m_previousState{};

		bool IsAnalogInput(int key) const;
		bool GetAnalogButtonState(int key, bool usePrevious) const;
		float GetRawAxisValue(int key, const GamepadState& state) const;
		float ApplyDeadzoneStick(float targetAxis, float otherAxis) const;

		SDL_Gamepad* FindGamepadByIndex(int index) const;
		bool IsRealGamepad(SDL_Gamepad* pad) const;

		// Convert platform-agnostic key to SDL button/axis
		static SDL_GamepadButton ToSDLButton(int key);
	};
}

dae::SDL3GamepadImpl::SDL3GamepadImpl(int controllerIdx)
	: m_controllerIdx(controllerIdx)
{
	m_pGamepad = FindGamepadByIndex(m_controllerIdx);
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
	{
		//TODO: This should be changed and be bound to a controller connected event instead of being polled every frame. (If you want hot-plug support ofc)
		m_pGamepad = FindGamepadByIndex(m_controllerIdx);
		if (!m_pGamepad)  
			return;
	}

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
		value = ApplyDeadzoneStick(state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTX)], state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTY)]);
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::LeftStickRight:
		value = ApplyDeadzoneStick(state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTX)], state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTY)]);
		value = value > 0.0f ? value : 0.0f;
		break;
	case GamepadInput::LeftStickUp:

		// SDL Y axis: negative is up
		value = ApplyDeadzoneStick(state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTY)], state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTX)]);
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::LeftStickDown:
		value = ApplyDeadzoneStick(state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTY)], state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_LEFTX)]);
		value = value > 0.0f ? value : 0.0f;
		break;

	case GamepadInput::RightStickLeft:
		value = ApplyDeadzoneStick(state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTX)], state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTY)]);
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::RightStickRight:
		value = ApplyDeadzoneStick(state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTX)], state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTY)]);
		value = value > 0.0f ? value : 0.0f;
		break;
	case GamepadInput::RightStickUp:
		value = ApplyDeadzoneStick(state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTY)], state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTX)]);
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::RightStickDown:
		value = ApplyDeadzoneStick(state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTY)], state.axes[static_cast<int>(SDL_GAMEPAD_AXIS_RIGHTX)]);
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

float dae::SDL3GamepadImpl::ApplyDeadzoneStick(float targetAxis, float otherAxis) const
{
	if (glm::length(glm::vec2(targetAxis, otherAxis)) >= m_deadzone)
		return targetAxis;

	return 0.0f;
}

SDL_Gamepad* dae::SDL3GamepadImpl::FindGamepadByIndex(int index) const
{
	SDL_Gamepad* gamepad{ nullptr };

	// Get available gamepads
	int numGamepads = 0;
	SDL_JoystickID* gamepads = SDL_GetGamepads(&numGamepads);

	if (gamepads)
	{
		int validIndex = 0;

		for (int i = 0; i < numGamepads; ++i)
		{
			SDL_JoystickID id = gamepads[i];

			if (!SDL_IsGamepad(id))
				continue;

			SDL_Gamepad* tempPad = SDL_OpenGamepad(id);
			if (!tempPad)
				continue;

			//Filter non gamepads since headsets, like my HyperX Cloud 3 wirelesss can connect as a gamepad. This way is pretty arbitrary.

			if (!IsRealGamepad(tempPad))
			{
				SDL_CloseGamepad(tempPad);
				continue;
			}

			if (validIndex == index)
			{
				gamepad = tempPad;
				break;
			}

			++validIndex;
			SDL_CloseGamepad(tempPad);
		}

		SDL_free(gamepads);
	}

	return gamepad;
}

bool dae::SDL3GamepadImpl::IsRealGamepad(SDL_Gamepad* pad) const
{
	if(!pad)
		return false;

	SDL_Joystick* joystick = SDL_GetGamepadJoystick(pad);
	if (!joystick)
		return false;

	int numButtons = SDL_GetNumJoystickButtons(joystick);
	int numAxes = SDL_GetNumJoystickAxes(joystick);

	if (numButtons < 4 || numAxes < 2) // Arbitrary thresholds for a "real" gamepad
		return false;

	return true;
}

float dae::SDL3GamepadImpl::GetAxisValue(int key) const
{
	if (!m_pGamepad)
		return 0.0f;

	float value = GetRawAxisValue(key, m_currentState);

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