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

	if (m_pActiveInputMap)
		m_pActiveInputMap->Evaluate();
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

void dae::Gamepad::LoadInputMaps(const Json& mapsJson)
{
	for (size_t mapIndex = 0; mapIndex < mapsJson.size(); ++mapIndex)
	{
		auto& map = mapsJson[mapIndex];

		// Validate map structure
		if (!map.contains("name"))
			throw std::invalid_argument("InputMap at index " + std::to_string(mapIndex) + " is missing required field: 'name'");
		if (!map.contains("bindings"))
			throw std::invalid_argument("InputMap '" + map["name"].get<std::string>() + "' is missing required field: 'bindings'");

		std::string name = map["name"];
		auto inputMap = std::make_unique<InputMap>();

		for (size_t bindingIndex = 0; bindingIndex < map["bindings"].size(); ++bindingIndex)
		{
			auto& action = map["bindings"][bindingIndex];

			// Validate binding structure
			if (!action.contains("name"))
				throw std::invalid_argument("Binding at index " + std::to_string(bindingIndex) + " in map '" + name + "' is missing required field: 'name'");
			if (!action.contains("type"))
				throw std::invalid_argument("Binding '" + action["name"].get<std::string>() + "' in map '" + name + "' is missing required field: 'type'");

			std::string bindingName = action["name"];
			std::string bindingType = action["type"];

			if (bindingType == "action")
			{
				if (!action.contains("key"))
					throw std::invalid_argument("Action binding '" + bindingName + "' in map '" + name + "' is missing required field: 'key'");
				if (!action.contains("state"))
					throw std::invalid_argument("Action binding '" + bindingName + "' in map '" + name + "' is missing required field: 'state'");

				int key = static_cast<int>(action["key"].get<GamepadInput>());

				InputState state = action["state"].get<InputState>();
				inputMap->CreateActionBinding(bindingName, key, state);
			}
			else if (bindingType == "axis")
			{
				if (!action.contains("positiveKey"))
					throw std::invalid_argument("Axis binding '" + bindingName + "' in map '" + name + "' is missing required field: 'positiveKey'");
				if (!action.contains("negativeKey"))
					throw std::invalid_argument("Axis binding '" + bindingName + "' in map '" + name + "' is missing required field: 'negativeKey'");

				int positiveKey = static_cast<int>(action["positiveKey"].get<GamepadInput>());
				int negativeKey = static_cast<int>(action["negativeKey"].get<GamepadInput>());
				inputMap->CreateAxisBinding(bindingName, positiveKey, negativeKey);
			}
			else if (bindingType == "axis2D")
			{
				if (!action.contains("upKey"))
					throw std::invalid_argument("Axis2D binding '" + bindingName + "' in map '" + name + "' is missing required field: 'upKey'");
				if (!action.contains("downKey"))
					throw std::invalid_argument("Axis2D binding '" + bindingName + "' in map '" + name + "' is missing required field: 'downKey'");
				if (!action.contains("leftKey"))
					throw std::invalid_argument("Axis2D binding '" + bindingName + "' in map '" + name + "' is missing required field: 'leftKey'");
				if (!action.contains("rightKey"))
					throw std::invalid_argument("Axis2D binding '" + bindingName + "' in map '" + name + "' is missing required field: 'rightKey'");

				int upKey = static_cast<int>(action["upKey"].get<GamepadInput>());
				int downKey = static_cast<int>(action["downKey"].get<GamepadInput>());
				int leftKey = static_cast<int>(action["leftKey"].get<GamepadInput>());
				int rightKey = static_cast<int>(action["rightKey"].get<GamepadInput>());
				inputMap->CreateAxis2DBinding(bindingName, leftKey, rightKey, downKey, upKey);
			}
			else
			{
				throw std::invalid_argument("Unknown binding type '" + bindingType + "' in binding '" + bindingName + "' (map: '" + name + "')");
			}
		}

		AddInputMap(name, std::move(inputMap));
	}
}
