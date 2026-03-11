#include "MoveCommands.h"

dae::Move2DCommand::Move2DCommand(GameActor& actor)
	: m_actor(&actor), m_cachedSpeed(actor.GetSpeed())
{
}

void dae::Move2DCommand::Execute()
{
	m_actor->Move(glm::vec2(GetAxisValue().x, GetAxisValue().y));
	m_actor->SetSpeed(m_cachedSpeed * glm::length(GetAxisValue()));
}
