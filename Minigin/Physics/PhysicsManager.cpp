#include "PhysicsManager.h"
#include "Rigidbody.h"
#include "Collider.h"
#include "Minigin/GameObject.h"
#include "Minigin/Transform.h"
#include <algorithm>
#include <cmath>

void dae::PhysicsManager::RegisterCollider(Collider* collider)
{
	if (!collider) return;
	const auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);
	if (it == m_colliders.end())
	{
		m_colliders.push_back(collider);
	}
}

void dae::PhysicsManager::UnregisterCollider(Collider* collider)
{
	if (!collider) return;
	const auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);
	if (it != m_colliders.end())
	{
		m_colliders.erase(it);
	}
}

void dae::PhysicsManager::RegisterRigidbody(Rigidbody* rigidbody)
{
    if (!rigidbody) return;
    const auto it = std::find(m_rigidbodies.begin(), m_rigidbodies.end(), rigidbody);
    if (it == m_rigidbodies.end())
    {
        m_rigidbodies.push_back(rigidbody);
    }
}

void dae::PhysicsManager::UnregisterRigidbody(Rigidbody* rigidbody)
{
	if (!rigidbody) return;
	const auto it = std::find(m_rigidbodies.begin(), m_rigidbodies.end(), rigidbody);
	if (it != m_rigidbodies.end())
	{
		m_rigidbodies.erase(it);
	}
}

void dae::PhysicsManager::Clear()
{
	m_colliders.clear();
	m_rigidbodies.clear();
}

void dae::PhysicsManager::FixedUpdate(float fixedDeltaTime)
{
	for (auto& rb : m_rigidbodies) {
		ResolveMovement(rb, fixedDeltaTime);
	}

	DispatchEvents();
}

dae::AABB dae::PhysicsManager::BoxAt(const Rigidbody& body)
{
    return BoxAt(*body.GetCollider(), body.GetOwner()->GetTransform().GetWorldPosition());
}

dae::AABB dae::PhysicsManager::BoxAt(const Rigidbody& body, const glm::vec2& pos)
{
	return BoxAt(*body.GetCollider(), pos);
}

dae::AABB dae::PhysicsManager::BoxAt(const Collider& col)
{
    return BoxAt(col, col.GetOwner()->GetTransform().GetWorldPosition());
}

dae::AABB dae::PhysicsManager::BoxAt(const Collider& col, const glm::vec2& pos)
{
    AABB box{col.GetBounds()};
    box.min += pos;
    box.max += pos;

    return box;
}

// Inspired by: "Swept AABB Collision Detection and Response" by BrendanL.K (from gamedev.net)
// https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/

dae::Hit dae::PhysicsManager::SweepAABB(const AABB& moving, const glm::vec2& velocity, const AABB& target)
{
    Hit result;
    result.hit = false;
    result.t = 1.0f;

    // Check if separated on a stationary axis - no collision possible
    if (velocity.x == 0.0f && (moving.max.x < target.min.x || moving.min.x > target.max.x)) {
        return result;
    }

    if (velocity.y == 0.0f && (moving.max.y < target.min.y || moving.min.y > target.max.y)) {
        return result;
    }

    glm::vec2 invEntry, invExit;

    if (velocity.x > 0.0f) {
        invEntry.x = target.min.x - moving.max.x;
        invExit.x = target.max.x - moving.min.x;
    }
    else {
        invEntry.x = target.max.x - moving.min.x;
        invExit.x = target.min.x - moving.max.x;
    }

    if (velocity.y > 0.0f) {
        invEntry.y = target.min.y - moving.max.y;
        invExit.y = target.max.y - moving.min.y;
    }
    else {
        invEntry.y = target.max.y - moving.min.y;
        invExit.y = target.min.y - moving.max.y;
    }

    glm::vec2 entry, exit;

    entry.x = (velocity.x == 0.0f) ? -INFINITY : invEntry.x / velocity.x;
    exit.x = (velocity.x == 0.0f) ? INFINITY : invExit.x / velocity.x;

    entry.y = (velocity.y == 0.0f) ? -INFINITY : invEntry.y / velocity.y;
    exit.y = (velocity.y == 0.0f) ? INFINITY : invExit.y / velocity.y;

    float entryTime = std::max(entry.x, entry.y);
    float exitTime = std::min(exit.x, exit.y);

    // No collision
    if (entryTime > exitTime || entryTime > 1.0f || (entry.x < 0.0f && entry.y < 0.0f)) {
        return result;
    }

    // Collision happened
    result.hit = true;
    result.t = entryTime;

    // Determine normal
    if (entry.x > entry.y) {
        result.normal = (invEntry.x < 0.0f) ? glm::vec2{ 1,0 } : glm::vec2{ -1,0 };
    }
    else {
        result.normal = (invEntry.y < 0.0f) ? glm::vec2{ 0,1 } : glm::vec2{ 0,-1 };
    }

    return result;
}

void dae::PhysicsManager::ResolveMovement(Rigidbody* rb, float deltaTime)
{
    glm::vec2 velocity = rb->GetVelocity();

    glm::vec2 remaining = velocity * deltaTime;
    glm::vec2 pos = rb->GetOwner()->GetTransform().GetWorldPosition();

    for (int i = 0; i < 4; i++) 
    {
        Hit bestHit{};
        bestHit.t = 1.0f;

        // Find earliest collision
        for (auto& collider : m_colliders) 
        {
            Hit h = SweepAABB(BoxAt(*rb, pos), remaining, BoxAt(*collider));

            if (h.hit && h.t < bestHit.t) 
            {
				h.b = collider;
                bestHit = h;
            }
        }

        if (!bestHit.hit) 
        {
            pos += remaining;
            break;
        }

        bestHit.a = rb->GetCollider();
		m_collisionEvents.push_back(bestHit);

        // Move to collision point
        pos += remaining * bestHit.t;

        remaining = remaining * (1.0f - bestHit.t);

		//This is a bounce where no energy gets lost, so the velocity is reflected across the normal of the collision surface.
        if (rb->CanBounce())
        {
            if (std::abs(bestHit.normal.x) > 0.0001f) remaining.x = -remaining.x;
            if (std::abs(bestHit.normal.y) > 0.0001f) remaining.y = -remaining.y;

            // Reflect velocity
            glm::vec2 reflected = velocity - 2.0f * glm::dot(velocity, bestHit.normal) * bestHit.normal;
			rb->SetVelocity(reflected);
        }
        // Slide
        remaining = remaining - glm::dot(remaining, bestHit.normal) * bestHit.normal;
        // Small push to avoid sticking
        pos += bestHit.normal * 0.001f;
    }

	rb->GetOwner()->GetTransform().SetWorldPosition({ pos.x, pos.y, rb->GetOwner()->GetTransform().GetWorldPosition().z });
}

void dae::PhysicsManager::DispatchEvents()
{
    for(auto& event : m_collisionEvents)
    {
		Collider* colA = event.a;
        Collider* colB = event.b;
        if (colA && colB) 
        {
            colA->OnCollisionEnter().NotifyObservers(event);
            colB->OnCollisionEnter().NotifyObservers(event);
		}
    }

	m_collisionEvents.clear();
}
