#include "Gamepad.h"
#include "GamepadImpl.h"
#include "Input/InputMap.h"

#ifdef __EMSCRIPTEN__
// SDL backend for non-Windows platforms
#include "SDL3GamepadImpl.cpp"
using GamepadImplType = dae::SDL3GamepadImpl;
#else
	#include "XInputGamepadImpl.cpp"
	using GamepadImplType = dae::XInputGamepadImpl;
#endif

dae::Gamepad::Gamepad(int controllerIdx)
	: m_pImpl(std::make_unique<GamepadImplType>(controllerIdx))
{
}

dae::Gamepad::~Gamepad() = default;

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
