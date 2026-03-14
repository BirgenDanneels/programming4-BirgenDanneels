#include "GamepadImpl.h"
#include "GamepadInput.h"
#include "Bindings/InputStates.h"
#include <Windows.h>
#include <Xinput.h>
#include <cmath>

namespace dae
{
	class XInputGamepadImpl : public GamepadImpl
	{
	public:
		explicit XInputGamepadImpl(int controllerIdx);
		~XInputGamepadImpl() override = default;

		void Update() override;
		bool GetKeyState(int key, InputState state) const override;
		float GetAxisValue(int key) const override;
		bool IsConnected() const override { return m_isConnected; }

		void SetDeadzone(float deadzone) override { m_deadzone = deadzone; }
		void SetAnalogToButtonThreshold(float threshold) override { m_analogToButtonThreshold = threshold; }

	private:
		int m_controllerIdx;

		XINPUT_STATE m_currentState{};
		XINPUT_STATE m_previousState{};
		bool m_isConnected{ false };

		bool IsAnalogInput(int key) const;
		bool GetAnalogButtonState(int key, bool usePrevious) const;
		float GetRawAxisValue(int key, bool usePrevious) const;

		// Convert platform-agnostic key to XInput button
		static int ToXInputButton(int key);
	};
}

dae::XInputGamepadImpl::XInputGamepadImpl(int controllerIdx)
	: m_controllerIdx(controllerIdx)
{
	ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_previousState, sizeof(XINPUT_STATE));
}

void dae::XInputGamepadImpl::Update()
{
	m_previousState = m_currentState;
	ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));

	DWORD result = XInputGetState(m_controllerIdx, &m_currentState);
	m_isConnected = (result == ERROR_SUCCESS);
}

int dae::XInputGamepadImpl::ToXInputButton(int key)
{
	switch (static_cast<GamepadInput>(key))
	{
	case GamepadInput::DPadUp: return XINPUT_GAMEPAD_DPAD_UP;
	case GamepadInput::DPadDown: return XINPUT_GAMEPAD_DPAD_DOWN;
	case GamepadInput::DPadLeft: return XINPUT_GAMEPAD_DPAD_LEFT;
	case GamepadInput::DPadRight: return XINPUT_GAMEPAD_DPAD_RIGHT;
	case GamepadInput::Start: return XINPUT_GAMEPAD_START;
	case GamepadInput::Back: return XINPUT_GAMEPAD_BACK;
	case GamepadInput::LeftThumb: return XINPUT_GAMEPAD_LEFT_THUMB;
	case GamepadInput::RightThumb: return XINPUT_GAMEPAD_RIGHT_THUMB;
	case GamepadInput::LeftShoulder: return XINPUT_GAMEPAD_LEFT_SHOULDER;
	case GamepadInput::RightShoulder: return XINPUT_GAMEPAD_RIGHT_SHOULDER;
	case GamepadInput::A: return XINPUT_GAMEPAD_A;
	case GamepadInput::B: return XINPUT_GAMEPAD_B;
	case GamepadInput::X: return XINPUT_GAMEPAD_X;
	case GamepadInput::Y: return XINPUT_GAMEPAD_Y;
	default: return 0;
	}
}

bool dae::XInputGamepadImpl::GetKeyState(int key, InputState state) const
{
	if (!m_isConnected)
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
		int xInputKey = ToXInputButton(key);
		currentPressed = (m_currentState.Gamepad.wButtons & xInputKey) != 0;
		previousPressed = (m_previousState.Gamepad.wButtons & xInputKey) != 0;
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

float dae::XInputGamepadImpl::GetAxisValue(int key) const
{
	if (!m_isConnected)
		return 0.0f;

	float value{ GetRawAxisValue(key, false) };

	// Apply deadzone
	if (std::abs(value) < m_deadzone)
		return 0.0f;

	return value;
}

bool dae::XInputGamepadImpl::IsAnalogInput(int key) const
{
	return key > static_cast<int>(GamepadInput::Y);
}

bool dae::XInputGamepadImpl::GetAnalogButtonState(int key, bool usePrevious) const
{
	float value{ GetRawAxisValue(key, usePrevious)};

	// Apply deadzone before threshold comparison
	if (std::abs(value) < m_deadzone)
		return false;

	// Compare against threshold
	return value >= m_analogToButtonThreshold;
}

float dae::XInputGamepadImpl::GetRawAxisValue(int key, bool usePrevious) const
{
	const XINPUT_STATE& state = usePrevious ? m_previousState : m_currentState;

	float value = 0.0f;

	switch (static_cast<GamepadInput>(key))
	{
	case GamepadInput::LeftTrigger:
		value = static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f;
		break;
	case GamepadInput::RightTrigger:
		value = static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f;
		break;

	case GamepadInput::LeftStickLeft:
		value = static_cast<float>(state.Gamepad.sThumbLX) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::LeftStickRight:
		value = static_cast<float>(state.Gamepad.sThumbLX) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case GamepadInput::LeftStickUp:
		value = static_cast<float>(state.Gamepad.sThumbLY) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case GamepadInput::LeftStickDown:
		value = static_cast<float>(state.Gamepad.sThumbLY) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;

	case GamepadInput::RightStickLeft:
		value = static_cast<float>(state.Gamepad.sThumbRX) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;
	case GamepadInput::RightStickRight:
		value = static_cast<float>(state.Gamepad.sThumbRX) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case GamepadInput::RightStickUp:
		value = static_cast<float>(state.Gamepad.sThumbRY) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case GamepadInput::RightStickDown:
		value = static_cast<float>(state.Gamepad.sThumbRY) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;

	default:
		int xInputKey = ToXInputButton(key);
		return (state.Gamepad.wButtons & xInputKey) != 0;
	}

	return value;
}
