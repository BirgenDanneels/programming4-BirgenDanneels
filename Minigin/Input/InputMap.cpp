#include "InputMap.h"
#include "InputDevice.h"

void dae::InputMap::Evaluate()
{
	for (auto& binding : m_ActionBindings)
	{
		if (m_pDevice->GetKeyState(binding.m_key, binding.m_InputState))
			binding.m_pCommand->Execute();
	}

	for (auto& binding : m_AxisBindings)
	{
		float value = 0.f;

		if (m_pDevice->GetKeyState(binding.m_negative, InputState::Down))
			value -= 1.f;
		if (m_pDevice->GetKeyState(binding.m_positive, InputState::Down))
			value += 1.f;

		binding.m_pCommand->SetAxisValue(value);
		binding.m_pCommand->Execute();
	}

	for (auto& binding : m_Axis2DBindings)
	{
		glm::vec3 value{ 0.f };
		if (m_pDevice->GetKeyState(binding.m_negativeX, InputState::Down))
			value.x -= 1.f;
		if (m_pDevice->GetKeyState(binding.m_positiveX, InputState::Down))
			value.x += 1.f;
		if (m_pDevice->GetKeyState(binding.m_negativeY, InputState::Down))
			value.y -= 1.f;
		if (m_pDevice->GetKeyState(binding.m_positiveY, InputState::Down))
			value.y += 1.f;

		binding.m_pCommand->SetAxisValue(value);
		binding.m_pCommand->Execute();
	}
}
