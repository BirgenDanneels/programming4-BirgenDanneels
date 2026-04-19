#include "CharacterController.h"
#include "GameObject.h"

dae::CharacterController::CharacterController(GameObject& pOwner)
	: Component(pOwner)
{
}

void dae::CharacterController::Update(float deltaTime)
{
	float length = glm::length(m_direction);
	if (length < 0.001f)
		return;

	if (length > 1.0f)
	{
		m_direction = glm::normalize(m_direction);
	}

	glm::vec3 currentPos = GetOwner()->GetLocalPosition();
	currentPos += glm::vec3(m_direction.x * deltaTime * m_speed, m_direction.y * deltaTime * m_speed, 0);
	GetOwner()->SetLocalPosition(currentPos);

	//Reset direction after moving (since its kinematic)
	m_direction = glm::vec2(0.0f, 0.0f);
}

void dae::CharacterController::Move(const glm::vec2& direction)
{
	m_direction += direction;
}
