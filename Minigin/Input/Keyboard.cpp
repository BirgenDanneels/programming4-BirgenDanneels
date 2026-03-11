#include "Keyboard.h"
#include "Input/InputMap.h"
#include <memory>

dae::Keyboard::Keyboard()
{
	//Get current state
	m_pCurrentState = SDL_GetKeyboardState(nullptr);
	for (int i = 0; i < SDL_SCANCODE_COUNT; ++i)
		m_previousState[i] = m_pCurrentState[i];
}

dae::Keyboard::~Keyboard() = default;

bool dae::Keyboard::IsKeyDown(int key) const
{
	return m_pCurrentState[key];
}

bool dae::Keyboard::IsKeyUp(int key) const
{
	return !m_pCurrentState[key];
}

bool dae::Keyboard::IsKeyPressed(int key) const
{
	return m_previousState[key] == false && m_pCurrentState[key];
}

bool dae::Keyboard::IsKeyReleased(int key) const
{
	return m_previousState[key] == true && !m_pCurrentState[key];
}

void dae::Keyboard::Update()
{
	if (m_pInputMap)
		m_pInputMap->Evaluate();

	for (int i = 0; i < SDL_SCANCODE_COUNT; ++i)
		m_previousState[i] = m_pCurrentState[i];
}

bool dae::Keyboard::GetKeyState(int key, InputState state) const
{
	switch (state)
	{
	case dae::InputState::Pressed:
		return IsKeyPressed(key);
		break;
	case dae::InputState::Released:
		return IsKeyReleased(key);
		break;
	case dae::InputState::Down:
		return IsKeyDown(key);
		break;
	case dae::InputState::Up:
		return IsKeyUp(key);
		break;
	default:
		return false;
		break;
	}
}

float dae::Keyboard::GetAxisValue(int key) const
{
	return IsKeyDown(key) ? 1.f : 0.f;
}
