#include "Keyboard.h"
#include "Input/InputMap.h"
#include <memory>
#include <stdexcept>

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
	if (m_pActiveInputMap)
		m_pActiveInputMap->Evaluate();

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

void dae::Keyboard::LoadInputMaps(const Json& mapsJson)
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

				int key = SDL_GetScancodeFromName(action["key"].get<std::string>().c_str());

				InputState state = action["state"].get<InputState>();
				inputMap->CreateActionBinding(bindingName, key, state);
			}
			else if (bindingType == "axis")
			{
				if (!action.contains("positiveKey"))
					throw std::invalid_argument("Axis binding '" + bindingName + "' in map '" + name + "' is missing required field: 'positiveKey'");
				if (!action.contains("negativeKey"))
					throw std::invalid_argument("Axis binding '" + bindingName + "' in map '" + name + "' is missing required field: 'negativeKey'");

				int positiveKey = SDL_GetScancodeFromName(action["positiveKey"].get<std::string>().c_str());
				int negativeKey = SDL_GetScancodeFromName(action["negativeKey"].get<std::string>().c_str());
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

				int upKey = SDL_GetScancodeFromName(action["upKey"].get<std::string>().c_str());
				int downKey = SDL_GetScancodeFromName(action["downKey"].get<std::string>().c_str());
				int leftKey = SDL_GetScancodeFromName(action["leftKey"].get<std::string>().c_str());
				int rightKey = SDL_GetScancodeFromName(action["rightKey"].get<std::string>().c_str());
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
