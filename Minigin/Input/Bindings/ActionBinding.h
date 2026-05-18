#include <memory>
#include "Minigin/Commands/Command.h"
#include "Minigin/Input/Bindings/InputStates.h"
#include <string>
#include <vector>

namespace dae
{
	struct ActionBinding
	{
		std::string m_name{};
		int m_key{ -1 };
		InputState m_InputState{ InputState::Pressed };
		std::vector<Command*> m_commands{};

		ActionBinding(const std::string& name, int key, InputState state)
			: m_name(name), m_key(key), m_InputState(state){}

		void AddCommand(Command& command)
		{
			m_commands.push_back(&command);
		}
	};

	struct Axis1DBinding
	{
		std::string m_name{};
		int m_negative{ -1 };
		int m_positive{ -1 };

		std::vector<Axis1DCommand*> m_commands{};

		Axis1DBinding(const std::string& name, int negative, int positive)
			: m_name(name), m_negative(negative), m_positive(positive)
		{
		}

		void AddCommand(Axis1DCommand& command)
		{
			m_commands.push_back(&command);
		}
	};

	struct Axis2DBinding
	{
		std::string m_name{};
		int m_negativeX{ -1 };
		int m_positiveX{ -1 };
		int m_negativeY{ -1 };
		int m_positiveY{ -1 };

		std::vector<Axis2DCommand*> m_commands{};

		Axis2DBinding(const std::string& name, int negativeX, int positiveX, int negativeY, int positiveY)
			: m_name(name), m_negativeX(negativeX), m_positiveX(positiveX), m_negativeY(negativeY), m_positiveY(positiveY)
		{
		}

		void AddCommand(Axis2DCommand& command)
		{
			m_commands.push_back(&command);
		}
	};
}