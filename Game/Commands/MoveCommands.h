#pragma once
#include "Minigin/Commands/Command.h"

namespace dae
{
	class CharacterController;

	class Move2DCommand : public Axis2DCommand
	{
	private:
		CharacterController* m_characterController{ nullptr };

	public:
		Move2DCommand(CharacterController& actor);
		void Execute() override;
	};

}