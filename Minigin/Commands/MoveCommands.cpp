#include "MoveCommands.h"
#include "Components/CharacterController.h"

dae::Move2DCommand::Move2DCommand(CharacterController& actor)
	: m_characterController(&actor)
{
}

void dae::Move2DCommand::Execute()
{
	if (!(glm::length(GetAxisValue()) > 0.f)) return;

	m_characterController->Move(glm::vec2(GetAxisValue().x, GetAxisValue().y));
}
