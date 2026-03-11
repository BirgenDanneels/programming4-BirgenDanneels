#include "Gamepad.h"
#include "Input/inputMap.h"

dae::Gamepad::Gamepad(int controllerIdx)
	: m_controllerIdx(controllerIdx)
{
	ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_previousState, sizeof(XINPUT_STATE));
}

void dae::Gamepad::Update()
{
	m_previousState = m_currentState;
	ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));

	DWORD result = XInputGetState(m_controllerIdx, &m_currentState);
	m_isConnected = (result == ERROR_SUCCESS);

	if (m_pInputMap)
		m_pInputMap->Evaluate();
}

bool dae::Gamepad::GetKeyState(int key, InputState state) const
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
		currentPressed = (m_currentState.Gamepad.wButtons & key) != 0;
		previousPressed = (m_previousState.Gamepad.wButtons & key) != 0;
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

float dae::Gamepad::GetAxisValue(int key) const
{
	if (!m_isConnected)
		return 0.0f;

	float value = 0.0f;

	switch (static_cast<Input>(key))
	{
	case Input::LeftTrigger:
		value = static_cast<float>(m_currentState.Gamepad.bLeftTrigger) / 255.0f;
		break;
	case Input::RightTrigger:
		value = static_cast<float>(m_currentState.Gamepad.bRightTrigger) / 255.0f;
		break;

	case Input::LeftStickLeft:
		value = static_cast<float>(m_currentState.Gamepad.sThumbLX) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;
	case Input::LeftStickRight:
		value = static_cast<float>(m_currentState.Gamepad.sThumbLX) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case Input::LeftStickUp:
		value = static_cast<float>(m_currentState.Gamepad.sThumbLY) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case Input::LeftStickDown:
		value = static_cast<float>(m_currentState.Gamepad.sThumbLY) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;

	case Input::RightStickLeft:
		value = static_cast<float>(m_currentState.Gamepad.sThumbRX) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;
	case Input::RightStickRight:
		value = static_cast<float>(m_currentState.Gamepad.sThumbRX) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case Input::RightStickUp:
		value = static_cast<float>(m_currentState.Gamepad.sThumbRY) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case Input::RightStickDown:
		value = static_cast<float>(m_currentState.Gamepad.sThumbRY) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;

	default:
		return (m_currentState.Gamepad.wButtons & key) != 0;
	}

	// Apply deadzone
	if (std::abs(value) < m_deadzone)
		return 0.0f;

	return value;
}

bool dae::Gamepad::IsAnalogInput(int key) const
{
	return key > static_cast<int>(Input::Y);
}

bool dae::Gamepad::GetAnalogButtonState(int key, bool usePrevious) const
{
	const XINPUT_STATE& state = usePrevious ? m_previousState : m_currentState;
	float value = 0.0f;

	switch (static_cast<Input>(key))
	{
	case Input::LeftTrigger:
		value = static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f;
		break;
	case Input::RightTrigger:
		value = static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f;
		break;

	case Input::LeftStickLeft:
		value = static_cast<float>(state.Gamepad.sThumbLX) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;
	case Input::LeftStickRight:
		value = static_cast<float>(state.Gamepad.sThumbLX) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case Input::LeftStickUp:
		value = static_cast<float>(state.Gamepad.sThumbLY) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case Input::LeftStickDown:
		value = static_cast<float>(state.Gamepad.sThumbLY) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;

	case Input::RightStickLeft:
		value = static_cast<float>(state.Gamepad.sThumbRX) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;
	case Input::RightStickRight:
		value = static_cast<float>(state.Gamepad.sThumbRX) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case Input::RightStickUp:
		value = static_cast<float>(state.Gamepad.sThumbRY) / 32767.0f;
		value = value > 0.0f ? value : 0.0f;
		break;
	case Input::RightStickDown:
		value = static_cast<float>(state.Gamepad.sThumbRY) / 32767.0f;
		value = value < 0.0f ? -value : 0.0f;
		break;

	default:
		return false;
	}

	// Apply deadzone before threshold comparison
	if (std::abs(value) < m_deadzone)
		return false;

	// Compare against threshold
	return value >= m_analogToButtonThreshold;
}
