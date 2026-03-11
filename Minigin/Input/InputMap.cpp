#include "InputMap.h"
#include "InputDevice.h"
#include <algorithm>

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

void dae::InputMap::UnbindAction(const std::string& name)
{
	m_ActionBindings.erase(
		std::remove_if(m_ActionBindings.begin(), m_ActionBindings.end(),
			[&name](const ActionBinding& binding) { return binding.m_name == name; }),
		m_ActionBindings.end()
	);
}

void dae::InputMap::UnbindAxis(const std::string& name)
{
	m_AxisBindings.erase(
		std::remove_if(m_AxisBindings.begin(), m_AxisBindings.end(),
			[&name](const Axis1DBinding& binding) { return binding.m_name == name; }),
		m_AxisBindings.end()
	);
}

void dae::InputMap::UnbindAxis2D(const std::string& name)
{
	m_Axis2DBindings.erase(
		std::remove_if(m_Axis2DBindings.begin(), m_Axis2DBindings.end(),
			[&name](const Axis2DBinding& binding) { return binding.m_name == name; }),
		m_Axis2DBindings.end()
	);
}
