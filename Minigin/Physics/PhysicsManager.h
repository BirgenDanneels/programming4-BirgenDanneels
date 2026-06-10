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

	struct HitEvent {
		Collider* otherCollider;
		float t;        
		glm::vec2 normal;
	};

	struct RaycastHit
	{
		Collider* collider{ nullptr };
		glm::vec2 point{};
		glm::vec2 normal{};
		float distance{ 0.0f };
		float t{ 0.0f };
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

		// Clear all registered colliders and rigidbodies
		void Clear();

		void FixedUpdate(float fixedDeltaTime);

		// Raycast
		bool Raycast(const glm::vec2& origin, const glm::vec2& direction, float maxDistance, RaycastHit& outHit, const Collider* ignoreCollider = nullptr) const;

	private:

		struct ColliderPair {
			Collider* a;
			Collider* b;
		};

		struct InternalHit {
			Collider* a;
			Collider* b;
			bool hit;
			float t;        // time of impact (0–1)
			glm::vec2 normal;
		};

		AABB BoxAt(const Collider& col) const;
		AABB BoxAt(const Collider& col, const glm::vec2& pos) const;

		InternalHit SweepAABB(const AABB& moving, const glm::vec2& velocity, const AABB& target);
		std::vector<ColliderPair> GetColliderPairs();
		bool IsResolvable(const ColliderPair& pair) const;
		InternalHit ResolvePair(const ColliderPair& pair, float deltaTime);

		// Raycast
		bool RayIntersectsAABB(const glm::vec2& origin, const glm::vec2& direction, const AABB& box, float maxDistance, float& outDistance, glm::vec2& outNormal) const;

		void HandleResponse(const InternalHit& hit, Collider* collider, const glm::vec2& normal, float fixedDeltaTime);

		void DispatchEvents();

		static constexpr float s_defaultGravity = 9.81f;
		float m_gravity{ s_defaultGravity };

		std::vector<Collider*> m_colliders;

		std::vector<InternalHit> m_collisionEvents; // Store collision events to dispatch after processing all movements
	};
}