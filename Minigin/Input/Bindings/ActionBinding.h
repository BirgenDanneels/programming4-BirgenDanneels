#include <memory>
#include "Commands/Command.h"
#include "Input/Bindings/InputStates.h"
#include <string>

namespace dae
{
	struct ActionBinding
	{
		std::string m_name{};
		int m_key{ -1 };
		InputState m_InputState{ InputState::Pressed };
		Command* m_pCommand{ nullptr };

		ActionBinding(const std::string& name, int key, InputState state, Command& command)
			: m_name(name), m_key(key), m_InputState(state), m_pCommand(&command) {}
	};

	struct Axis1DBinding
	{
		std::string m_name{};
		int m_negative{ -1 };
		int m_positive{ -1 };

		Axis1DCommand* m_pCommand{ nullptr };

		Axis1DBinding(const std::string& name, int negative, int positive, Axis1DCommand& command)
			: m_name(name), m_negative(negative), m_positive(positive), m_pCommand(&command) { }
	};

	struct Axis2DBinding
	{
		std::string m_name{};
		int m_negativeX{ -1 };
		int m_positiveX{ -1 };
		int m_negativeY{ -1 };
		int m_positiveY{ -1 };

		Axis2DCommand* m_pCommand{ nullptr };

		Axis2DBinding(const std::string& name, int negativeX, int positiveX, int negativeY, int positiveY, Axis2DCommand& command)
			: m_name(name), m_negativeX(negativeX), m_positiveX(positiveX), m_negativeY(negativeY), m_positiveY(positiveY), m_pCommand(&command) { }
	};
}