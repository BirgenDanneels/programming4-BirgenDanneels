#include "Gamepad.h"
#include "GamepadImpl.h"
#include "Input/InputMap.h"

#ifdef _WIN32
	#include "XInputGamepadImpl.h"
	using GamepadImplType = dae::XInputGamepadImpl;
#else
	// SDL backend for non-Windows platforms
	#include "SDL3GamepadImpl.h"
	using GamepadImplType = dae::SDL3GamepadImpl;
#endif

dae::Gamepad::Gamepad(int controllerIdx)
	: m_pImpl(std::make_unique<GamepadImplType>(controllerIdx))
{
}

void dae::Gamepad::Update()
{
	m_pImpl->Update();

	if (m_pInputMap)
		m_pInputMap->Evaluate();
}

bool dae::Gamepad::GetKeyState(int key, InputState state) const
{
		return m_pImpl->GetKeyState(key, state);
}

float dae::Gamepad::GetAxisValue(int key) const
{
	return m_pImpl->GetAxisValue(key);
}

void dae::Gamepad::SetDeadzone(float deadzone)
{
	m_pImpl->SetDeadzone(deadzone);
}

void dae::Gamepad::SetAnalogToButtonThreshold(float threshold)
{
	m_pImpl->SetAnalogToButtonThreshold(threshold);
}
