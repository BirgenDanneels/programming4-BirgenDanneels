#pragma once

#include "Commands/GameActorCommand.h"

namespace dae
{
	class Move2DCommand : public Axis2DCommand
	{
	private:
		GameActor* m_actor{ nullptr };

	public:
		Move2DCommand(GameActor& actor);
		void Execute() override;
	};

}