#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace dae
{
	class Rigidbody;
	class Collider;
	class GameObject;

	struct AABB;

	struct Hit {
		Collider* a;
		Collider* b;
		bool hit;
		float t;        // time of impact (0–1)
		glm::vec2 normal;
	};

	class PhysicsManager final
	{
	public:

		PhysicsManager() = default;
		PhysicsManager(const PhysicsManager& other) = delete;
		PhysicsManager(PhysicsManager&& other) = delete;
		PhysicsManager& operator=(const PhysicsManager& other) = delete;
		PhysicsManager& operator=(PhysicsManager&& other) = delete;

		~PhysicsManager() = default;

		float GetGravity() { return m_gravity; }
		void SetGravity(float gravity) { m_gravity = gravity; }

		// Register/unregister colliders with the physics system
		void RegisterCollider(Collider* collider);
		void UnregisterCollider(Collider* collider);
		void RegisterRigidbody(Rigidbody* rigidbody);
		void UnregisterRigidbody(Rigidbody* rigidbody);

		// Clear all registered colliders and rigidbodies
		void Clear();

		void FixedUpdate(float fixedDeltaTime);

	private:

		AABB BoxAt(const Rigidbody& body);
		AABB BoxAt(const Rigidbody& body, const glm::vec2& pos);
		AABB BoxAt(const Collider& col);
		AABB BoxAt(const Collider& col, const glm::vec2& pos);
		Hit SweepAABB(const AABB& moving, const glm::vec2& velocity, const AABB& target);
		void ResolveMovement(Rigidbody* rb, float deltaTime);

		void DispatchEvents();

		static constexpr float s_defaultGravity = 9.81f;
		float m_gravity{ s_defaultGravity };

		std::vector<Collider*> m_colliders;
		std::vector<Rigidbody*> m_rigidbodies;

		std::vector<Hit> m_collisionEvents; // Store collision events to dispatch after processing all movements
	};
}