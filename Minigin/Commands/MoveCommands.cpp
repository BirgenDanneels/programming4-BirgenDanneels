#include "MoveCommands.h"

dae::Move2DCommand::Move2DCommand(GameActor& actor)
	: m_actor(&actor)
{
}

void dae::Move2DCommand::Execute()
{
	m_actor->Move(glm::vec2(GetAxisValue().x, GetAxisValue().y));
}
