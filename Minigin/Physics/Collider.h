#pragma once
#include <glm/glm.hpp>
#include "Minigin/Components/Component.h"
#include "Minigin/Events/Subject.h"
#include "Rigidbody.h"
#include "PhysicsManager.h"

namespace dae
{
	struct AABB {
		glm::vec2 min;
		glm::vec2 max;
	};

	class Collider final : public Component
	{
	public:
		Collider() = delete;
		Collider(GameObject& refOwner);

		Collider(const Collider& other) = delete;
		Collider(Collider&& other) = delete;
		Collider& operator=(const Collider& other) = delete;
		Collider& operator=(Collider&& other) = delete;

		~Collider() override;

		virtual void Start() override;
		virtual void FixedUpdate(float /*fixedDeltaTime*/) override {};
		virtual void Update(float /*deltaTime*/) override {};
		virtual void Render() const override {};
		virtual void RenderUI() override;

		void InitializeBoxCollider(float width, float height, float xOffset = 0.0f, float yOffset = 0.0f);
		//Loading functions
		virtual std::vector<ParamDefinition> GetExpectedParams() const override;
		virtual void Load(const ParamMap& params) override;

		const glm::vec2& GetOffset() const { return m_offset; }
		const glm::vec2& GetSize() const { return m_size; }
		float GetRadius() const { return m_size.x; }
		
		glm::vec2 GetWorldPosition() const;
		AABB GetBounds() const;

		void SetRigidbody(Rigidbody* rb) { m_pRigidbody = rb; }
		Rigidbody* GetRigidbody() const { return m_pRigidbody; }

		void SetDebugDraw(bool enable) { m_debugDraw = enable; }
		bool GetDebugDraw() const { return m_debugDraw; }

		// Events
		Subject<HitEvent>& OnCollision() { return m_onCollision; }

	private:
		glm::vec2 m_offset{ 0.0f, 0.0f };
		glm::vec2 m_size{ 1.0f, 1.0f };
		bool m_debugDraw{ true };

		Rigidbody* m_pRigidbody{ nullptr };

		// Event subjects
		Subject<HitEvent> m_onCollision;
	};
}