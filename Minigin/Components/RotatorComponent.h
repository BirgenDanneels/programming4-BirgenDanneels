#pragma once
#include "Component.h"
#include "GameObject.h"

namespace dae
{
	class RotatorComponent : public Component
	{
	public:
		RotatorComponent() = delete;
		RotatorComponent(dae::GameObject& refOwner);

		RotatorComponent(const RotatorComponent& other) = delete;
		RotatorComponent(RotatorComponent&& other) = delete;
		RotatorComponent& operator=(const RotatorComponent& other) = delete;
		RotatorComponent& operator=(RotatorComponent&& other) = delete;

		virtual ~RotatorComponent() = default;

		virtual void FixedUpdate(float deltaTime)override;
		virtual void Update(float deltaTime) override;
		virtual void Render() const override {};

		virtual void Initialize(dae::GameObject& targetObject, float rotationSpeed);

	private:

		glm::vec3 RotateAround(const glm::vec3& pivot, float angle) const;

		GameObject* m_ptrTargetObject;
		float m_rotationSpeed;
	};
}
