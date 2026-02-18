#pragma once
#include "Components/Component.h"
#include <glm/glm.hpp>

namespace dae
{

	class TransformComponent : public Component
	{
	public:

		TransformComponent(dae::GameObject& refOwner);

		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent(TransformComponent&& other) = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;
		TransformComponent& operator=(TransformComponent&& other) = delete;

		~TransformComponent() = default;

		virtual void Initialize(float x, float y);

		virtual void FixedUpdate(float fixedDeltaTime) override;
		virtual void Update(float deltaTime) override;
		virtual void Render() const override;

		const glm::vec3& GetPosition() const { return m_position; }
		void SetPosition(float x, float y, float z = 0.0f);
		void SetPosition(glm::vec3 pos);

	private:

		glm::vec3 m_position{};
	};

}