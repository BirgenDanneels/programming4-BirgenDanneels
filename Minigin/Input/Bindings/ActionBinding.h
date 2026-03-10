#include <memory>
#include "Commands/command.h"
#include "Input/Bindings/InputStates.h"

namespace dae
{
	struct ActionBinding
	{
		int m_key{ -1 };
		InputState m_InputState{ InputState::Pressed };
		std::unique_ptr<Command> m_pCommand{ nullptr };

		ActionBinding(int key, InputState state, std::unique_ptr<Command> command)
			: m_key(key), m_InputState(state), m_pCommand(std::move(command)){}
	};

	struct Axis1DBinding
	{
		int m_negative{ -1 };
		int m_positive{ -1 };

		std::unique_ptr<Axis1DCommand> m_pCommand{ nullptr };

		Axis1DBinding(int negative, int positive, std::unique_ptr<Axis1DCommand> command)
			:m_negative(negative), m_positive(positive), m_pCommand(std::move(command)) { }
	};

	struct Axis2DBinding
	{
		int m_negativeX{ -1 };
		int m_positiveX{ -1 };
		int m_negativeY{ -1 };
		int m_positiveY{ -1 };
		std::unique_ptr<Axis2DCommand> m_pCommand{ nullptr };
		Axis2DBinding(int negativeX, int positiveX, int negativeY, int positiveY, std::unique_ptr<Axis2DCommand> command)
			: m_negativeX(negativeX), m_positiveX(positiveX), m_negativeY(negativeY), m_positiveY(positiveY), m_pCommand(std::move(command)) { }
	};
}