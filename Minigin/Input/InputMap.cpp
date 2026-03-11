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

		value -= m_pDevice->GetAxisValue(binding.m_negative);
		value += m_pDevice->GetAxisValue(binding.m_positive);;

		binding.m_pCommand->SetAxisValue(value);
		binding.m_pCommand->Execute();
	}

	for (auto& binding : m_Axis2DBindings)
	{
		glm::vec3 value{ 0.f };

		value.x -= m_pDevice->GetAxisValue(binding.m_negativeX);
		value.x += m_pDevice->GetAxisValue(binding.m_positiveX);
		value.y -= m_pDevice->GetAxisValue(binding.m_negativeY);
		value.y += m_pDevice->GetAxisValue(binding.m_positiveY);

		binding.m_pCommand->SetAxisValue(value);
		binding.m_pCommand->Execute();
	}
}
