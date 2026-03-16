#pragma once
#include "Command.h"

namespace dae
{
	class CharacterController;

	class Move2DCommand : public Axis2DCommand
	{
	private:
		CharacterController* m_characterController{ nullptr };
		float m_cachedSpeed{ 100.f };

	public:
		Move2DCommand(CharacterController& actor);
		void Execute() override;
	};

}