#include "InputDevice.h"
#include "InputMap.h"

dae::InputDevice::~InputDevice() = default;

void dae::InputDevice::SetInputMap(std::unique_ptr<InputMap> inputMap)
{
	m_pInputMap = std::move(inputMap);
	if(m_pInputMap)
		m_pInputMap->SetDevice(this);
}