#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class GameActor : public Component
	{
		public:

			GameActor() = delete;
			GameActor(dae::GameObject& refOwner);

			GameActor(const GameActor& other) = delete;
			GameActor(GameActor&& other) = delete;
			GameActor& operator=(const GameActor& other) = delete;
			GameActor& operator=(GameActor&& other) = delete;

			~GameActor() = default;

			void Update(float deltaTime) override; //using this update since at this moment we arent using physics
			virtual void FixedUpdate(float deltaTime) override { (void)deltaTime; };
			virtual void Render() const override {};

			void SetSpeed(float speed) { m_speed = speed; }
			float GetSpeed() const { return m_speed; }
			void Move(const glm::vec2& direction);

		private:

			float m_speed{ 100.0f };
			glm::vec2 m_direction{ 0.0f, 0.0f };
	};
}