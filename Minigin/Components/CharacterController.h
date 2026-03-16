#include "Component.h"
#include <glm/glm.hpp>

namespace dae
{
	class CharacterController final : public Component
	{
	public:
		CharacterController(GameObject& pOwner);
		~CharacterController() override = default;


		void Update(float deltaTime) override;

		virtual void FixedUpdate(float fixedDeltaTime) override { (void)fixedDeltaTime; };
		virtual void Render() const override {};

		void SetSpeed(float speed) { m_speed = speed; }
		float GetSpeed() const { return m_speed; }
		void Move(const glm::vec2& direction);

	private:

		float m_speed{ 100.0f };
		glm::vec2 m_direction{ 0.0f, 0.0f };
	};
}