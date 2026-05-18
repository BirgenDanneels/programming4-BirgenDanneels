#pragma once
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cctype>

namespace dae
{
	enum class GamepadInput
	{
		DPadUp,
		DPadDown,
		DPadLeft,
		DPadRight,

		Start,
		Back,

		LeftThumb,
		RightThumb,
		LeftShoulder,
		RightShoulder,

		A,
		B,
		X,
		Y,

		//Analog inputs

		LeftStickUp,
		LeftStickDown,
		LeftStickLeft,
		LeftStickRight,

		RightStickUp,
		RightStickDown,
		RightStickLeft,
		RightStickRight,

		LeftTrigger,
		RightTrigger,
	};

	inline void from_json(const nlohmann::json& j, GamepadInput& input)
	{
		std::string value = j.get<std::string>();
		std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
			return std::tolower(c); });
		if (value == "dpadup") input = GamepadInput::DPadUp;
		else if (value == "dpaddown") input = GamepadInput::DPadDown;
		else if (value == "dpadleft") input = GamepadInput::DPadLeft;
		else if (value == "dpadright") input = GamepadInput::DPadRight;
		else if (value == "start") input = GamepadInput::Start;
		else if (value == "back") input = GamepadInput::Back;
		else if (value == "leftthumb") input = GamepadInput::LeftThumb;
		else if (value == "rightthumb") input = GamepadInput::RightThumb;
		else if (value == "leftshoulder") input = GamepadInput::LeftShoulder;
		else if (value == "rightshoulder") input = GamepadInput::RightShoulder;
		else if (value == "a") input = GamepadInput::A;
		else if (value == "b") input = GamepadInput::B;
		else if (value == "x") input = GamepadInput::X;
		else if (value == "y") input = GamepadInput::Y;
		else if (value == "leftstickup") input = GamepadInput::LeftStickUp;
		else if (value == "leftstickdown") input = GamepadInput::LeftStickDown;
		else if (value == "leftstickleft") input = GamepadInput::LeftStickLeft;
		else if (value == "leftstickright") input = GamepadInput::LeftStickRight;
		else if (value == "rightstickup") input = GamepadInput::RightStickUp;
		else if (value == "rightstickdown") input = GamepadInput::RightStickDown;
		else if (value == "rightstickleft") input = GamepadInput::RightStickLeft;
		else if (value == "rightstickright") input = GamepadInput::RightStickRight;
		else if (value == "lefttrigger") input = GamepadInput::LeftTrigger;
		else if (value == "righttrigger") input = GamepadInput::RightTrigger;
		else throw std::invalid_argument("Unknown GamepadInput: " + value);
	}
}