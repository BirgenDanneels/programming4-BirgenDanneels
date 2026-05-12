#include "CharacterController.h"
#include "GameObject.h"
#include "Loading/LoadingHelpers.h"

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

	GetOwner()->GetTransform().Translate(m_direction * deltaTime * m_speed);

	//Reset direction after moving (since its kinematic)
	m_direction = glm::vec2(0.0f, 0.0f);
}

void dae::CharacterController::Move(const glm::vec2& direction)
{
	m_direction += direction;
}

std::vector<dae::ParamDefinition> dae::CharacterController::GetExpectedParams() const
{
	return {
		{"Speed", 100.0f}
	};
}

void dae::CharacterController::Load(const ParamMap& params)
{
	m_speed = dae::GetRequiredParam<float>(params, "Speed");
}