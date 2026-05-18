#include "InputMap.h"
#include "InputDevice.h"
#include <algorithm>

dae::InputMap::InputMap() = default;

dae::InputMap::~InputMap() = default;

void dae::InputMap::Evaluate()
{
	for (auto& binding : m_ActionBindings)
	{
		if (m_pDevice->GetKeyState(binding->m_key, binding->m_InputState))
		{
			for (auto& command : binding->m_commands)
			{
				command->Execute();
			}
		}
	}

	for (auto& binding : m_AxisBindings)
	{
		float value = 0.f;

		value -= m_pDevice->GetAxisValue(binding->m_negative);
		value += m_pDevice->GetAxisValue(binding->m_positive);;

		value = std::clamp(value, -1.f, 1.f);

		for(auto& command : binding->m_commands)
		{
			command->SetAxisValue(value);
			command->Execute();
		}
	}

	for (auto& binding : m_Axis2DBindings)
	{
		glm::vec3 value{ 0.f };

		value.x -= m_pDevice->GetAxisValue(binding->m_negativeX);
		value.x += m_pDevice->GetAxisValue(binding->m_positiveX);
		value.y -= m_pDevice->GetAxisValue(binding->m_negativeY);
		value.y += m_pDevice->GetAxisValue(binding->m_positiveY);

		value = glm::clamp(value, glm::vec3(-1.f), glm::vec3(1.f));

		for (auto& command : binding->m_commands)
		{
			command->SetAxisValue(value);
			command->Execute();
		}
	}
}
