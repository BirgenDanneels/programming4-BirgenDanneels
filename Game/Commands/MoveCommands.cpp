#include "MoveCommands.h"
#include "Minigin/Physics/Rigidbody.h"

dae::Move2DCommand::Move2DCommand(dae::Rigidbody& rigidbody, float speed)
	: m_rigidbody(&rigidbody), m_speed(speed)
{
}

void dae::Move2DCommand::Execute()
{
	if (!(glm::length(GetAxisValue()) > 0.f))
	{
		if(!m_isMoving)
			return;

		m_rigidbody->SetVelocity({ 0.f, 0.f });
		m_isMoving = false;
		return;
	}

	m_rigidbody->SetVelocity(glm::normalize(GetAxisValue()) * m_speed);
	m_isMoving = true;
}
