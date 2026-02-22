#include "TransformComponent.h"

dae::TransformComponent::TransformComponent(dae::GameObject& refOwner)
	:Component(refOwner)
{
}

void dae::TransformComponent::Initialize(float x, float y)
{
	m_position.r = x;
	m_position.g = y;
}

void dae::TransformComponent::FixedUpdate(float fixedDeltaTime)
{
	(void)fixedDeltaTime;
}

void dae::TransformComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::TransformComponent::Render() const
{
}

void dae::TransformComponent::SetPosition(float x, float y, float z)
{
	m_position.r = x;
	m_position.g = y;
	m_position.b = z;
}

void dae::TransformComponent::SetPosition(glm::vec3 pos)
{
	m_position = pos;
}
