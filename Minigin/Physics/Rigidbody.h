#pragma once
#include <glm/glm.hpp>
#include "Minigin/Components/Component.h"

namespace dae
{
	class Rigidbody final : public Component
	{
	public:
		Rigidbody() = delete;
		explicit Rigidbody(GameObject& refOwner);

		Rigidbody(const Rigidbody& other) = delete;
		Rigidbody(Rigidbody&& other) = delete;
		Rigidbody& operator=(const Rigidbody& other) = delete;
		Rigidbody& operator=(Rigidbody&& other) = delete;

		~Rigidbody() override;

		virtual void Start() override;
		virtual void FixedUpdate(float) override;
		virtual void Update(float) override {};
		virtual void Render() const override {};

		// Initialization
		void Initialize(bool useGravity = false, bool isKinematic = true, bool canBounce = false);
		//Loading functions
		virtual std::vector<ParamDefinition> GetExpectedParams() const override;
		virtual void Load(const ParamMap& params) override;


		// Movement
		void SetVelocity(const glm::vec2& velocity);
		void SetVelocity(float x, float y) { m_velocity = { x, y }; }
		const glm::vec2& GetVelocity() const { return m_velocity; }

		void AddForce(const glm::vec2& force);

		// Properties
		void SetGravityScale(float scale) { m_gravityScale = scale; }
		float GetGravityScale() const { return m_gravityScale; }

		void SetIsKinematic(bool kinematic) { m_isKinematic = kinematic; }
		bool IsKinematic() const { return m_isKinematic; }

		void SetUseGravity(bool useGravity) { m_useGravity = useGravity; }
		bool GetUseGravity() const { return m_useGravity; }

		void SetDrag(float drag);
		float GetDrag() const { return m_drag; }

		void SetCanBounce(bool canBounce) { m_canBounce = canBounce; }
		bool CanBounce() const { return m_canBounce; }

		void SetConstraints(bool freezeX = false, bool freezeY = false);
		bool IsFrozenX() const { return m_freezeX; }
		bool IsFrozenY() const { return m_freezeY; }

	private:

		// Movement and physics
		glm::vec2 m_velocity{ 0.0f, 0.0f };
		glm::vec2 m_acceleration{ 0.0f, 0.0f };

		float m_gravityScale{ 1.0f };
		float m_drag{ 0.0f };

		bool m_isKinematic{ true };
		bool m_useGravity{ false };
		bool m_canBounce{ false };

		bool m_freezeX{ false };
		bool m_freezeY{ false };
	};
}