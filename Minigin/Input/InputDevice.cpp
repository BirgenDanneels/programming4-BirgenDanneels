#include "InputDevice.h"
#include "InputMap.h"
#include <algorithm>

dae::InputDevice::~InputDevice() = default;

void dae::InputDevice::AddInputMap(std::string name, std::unique_ptr<InputMap> inputMap)
{
	m_inputMaps[name] = std::move(inputMap);
	if (m_inputMaps[name])
		m_inputMaps[name]->SetDevice(this);
}

void dae::InputDevice::SetActiveInputMap(std::string name)
{
	auto it = m_inputMaps.find(name);
	if (it != m_inputMaps.end())
	{
		m_pActiveInputMap = it->second.get();
	}
	else
	{
		m_pActiveInputMap = nullptr;
	}
}

dae::InputMap* dae::InputDevice::GetInputMap(std::string name) const
{
	auto it = m_inputMaps.find(name);
	if (it != m_inputMaps.end())
	{
		return it->second.get();
	}
	return nullptr;
};

void dae::InputDevice::RemoveInputMap(std::string name)
{
	auto it = m_inputMaps.find(name);
	if (it != m_inputMaps.end())
	{
		if (m_pActiveInputMap == it->second.get())
			m_pActiveInputMap = nullptr;
		m_inputMaps.erase(it);
	}
}