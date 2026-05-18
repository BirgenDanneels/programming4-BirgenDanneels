#pragma once
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cctype>

namespace dae
{
	enum class InputState
	{
		Pressed,
		Released,
		Down,
		Up
	};

	inline void from_json(const nlohmann::json& j, InputState& state)
	{
		std::string value = j.get<std::string>();
		std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
			return std::tolower(c); });
		
		if (value == "pressed") state = InputState::Pressed;
		else if (value == "released") state = InputState::Released;
		else if (value == "down") state = InputState::Down;
		else if (value == "up") state = InputState::Up;
		else throw std::invalid_argument("Unknown InputState: " + value);
	}
}