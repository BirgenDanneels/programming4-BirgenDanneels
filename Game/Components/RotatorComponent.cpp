#define _USE_MATH_DEFINES
#include <cmath>
#include "RotatorComponent.h"

RotatorComponent::RotatorComponent(dae::GameObject& refOwner)
	:dae::Component(refOwner), m_rotationSpeed{ 0 }
{
}

void RotatorComponent::Update(float deltaTime)
{
	float rotationSpeed{ 360.f * m_rotationSpeed * float(deltaTime) };

	if (dae::GameObject* parent{GetOwner()->GetParent()})
		GetOwner()->GetTransform().SetWorldPosition(RotateAround(parent->GetTransform().GetWorldPosition(), rotationSpeed));
}

void RotatorComponent::Initialize(float rotationSpeed)
{
	m_rotationSpeed = rotationSpeed;
}

glm::vec3 RotatorComponent::RotateAround(const glm::vec3& pivot, float angle) const
{
	const float radians{ angle * float(M_PI) / 180.f };
	const glm::vec3 position{ GetOwner()->GetTransform().GetWorldPosition() };

	const float translatedX = position.x - pivot.x;
	const float translatedY = position.y - pivot.y;

	const float cosTheta{ std::cos(radians) };
	const float sinTheta{ std::sin(radians) };

	const float rotatedX{ translatedX * cosTheta - translatedY * sinTheta };
	const float rotatedY{ translatedX * sinTheta + translatedY * cosTheta };

	return glm::vec3(rotatedX + pivot.x, rotatedY + pivot.y, position.z);

}
