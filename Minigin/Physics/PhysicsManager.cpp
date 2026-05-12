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

void dae::PhysicsManager::Clear()
{
	m_colliders.clear();
}

void dae::PhysicsManager::FixedUpdate(float fixedDeltaTime)
{
    std::vector<ColliderPair> pairs = GetColliderPairs();

	float remaining = 1;

    while (remaining > 0)
    {
		InternalHit bestHit{};
        bestHit.t = 1.0f;

		for (ColliderPair& pair : pairs)
        {
            if (!IsResolvable(pair))
                continue;

			InternalHit hit = ResolvePair(pair, fixedDeltaTime * remaining);

            if (hit.hit && hit.t < bestHit.t)
                bestHit = hit;

        }

        if (bestHit.hit)
        {
            m_collisionEvents.push_back(bestHit);

			// Handle response for both objects with correct normals
			HandleResponse(bestHit, bestHit.a, bestHit.normal, fixedDeltaTime * remaining);
            HandleResponse(bestHit, bestHit.b, -bestHit.normal, fixedDeltaTime * remaining);

            for (Collider* c : m_colliders)
            {
				if (c == bestHit.a || c == bestHit.b)
					continue;

                if (Rigidbody* rb = c->GetRigidbody())
                {
                    glm::vec2 pos = rb->GetOwner()->GetTransform().GetWorldPosition();
                    pos += rb->GetVelocity() * fixedDeltaTime * remaining * bestHit.t;
                    rb->GetOwner()->GetTransform().SetWorldPosition({ pos.x, pos.y, rb->GetOwner()->GetTransform().GetWorldPosition().z });
                }
            }

            remaining *= (1.0f - bestHit.t);
        }
        else
        {
            for (Collider* c : m_colliders)
            {
                if (Rigidbody* rb = c->GetRigidbody())
                {
                    glm::vec2 pos = rb->GetOwner()->GetTransform().GetWorldPosition();
                    pos += rb->GetVelocity() * fixedDeltaTime * remaining;
                    rb->GetOwner()->GetTransform().SetWorldPosition({ pos.x, pos.y, rb->GetOwner()->GetTransform().GetWorldPosition().z});
                }
            }
            break;
		}
    }

	DispatchEvents();
}

void dae::PhysicsManager::HandleResponse(const InternalHit& hit, Collider* collider, const glm::vec2& normal, float fixedDeltaTime)
{
    Rigidbody* rb = collider->GetRigidbody();
	if (!rb) return;

	glm::vec2 pos = rb->GetOwner()->GetTransform().GetWorldPosition();
	glm::vec2 velocity = rb->GetVelocity();

    // Move to collision point
    pos += velocity * fixedDeltaTime * hit.t;

    if (rb->CanBounce())
    {
        // Reflect velocity across the normal (preserve magnitude)
        glm::vec2 reflected = velocity - 2.0f * glm::dot(velocity, normal) * normal;
        rb->SetVelocity(reflected);
    }
    else
    {
        // Slide: remove velocity perpendicular to surface
        glm::vec2 slidVel = velocity - glm::dot(velocity, normal) * normal;
        rb->SetVelocity(slidVel);
    }

    pos += normal * 0.01f;
    rb->GetOwner()->GetTransform().SetWorldPosition({ pos.x, pos.y, rb->GetOwner()->GetTransform().GetWorldPosition().z });
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

dae::PhysicsManager::InternalHit dae::PhysicsManager::SweepAABB(const AABB& moving, const glm::vec2& velocity, const AABB& target)
{
    InternalHit result;
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

std::vector<dae::PhysicsManager::ColliderPair> dae::PhysicsManager::GetColliderPairs()
{
    std::vector<ColliderPair> pairs;
    for (size_t i = 0; i < m_colliders.size(); ++i)
    {
        for (size_t j = i + 1; j < m_colliders.size(); ++j)
        {
            pairs.push_back({ m_colliders[i], m_colliders[j] });
        }
    }
    return pairs;
}

bool dae::PhysicsManager::IsResolvable(const ColliderPair& pair) const
{
    if (!pair.a->GetRigidbody() && !pair.b->GetRigidbody())
        return false;

    return true;
}

dae::PhysicsManager::InternalHit dae::PhysicsManager::ResolvePair(const ColliderPair& pair, float remainingDeltaTime)
{
    glm::vec2 velA{ 0,0 };
    glm::vec2 velB{ 0,0 };
    if (pair.a->GetRigidbody())
    {
        velA = pair.a->GetRigidbody()->GetVelocity();
    }
    if (pair.b->GetRigidbody())
    {
        velB = pair.b->GetRigidbody()->GetVelocity();
    }

    glm::vec2 relativeVel = velA - velB;
    relativeVel *= remainingDeltaTime;

    InternalHit hit = SweepAABB(BoxAt(*pair.a), relativeVel, BoxAt(*pair.b));
	hit.a = pair.a;
	hit.b = pair.b;

	return hit;
}

void dae::PhysicsManager::DispatchEvents()
{

    for(auto& event : m_collisionEvents)
    {
		Collider* colA = event.a;
        Collider* colB = event.b;
        if (colA && colB) 
        {
            colA->OnCollision().NotifyObservers({event.b, event.t, event.normal});
            colB->OnCollision().NotifyObservers({event.a, event.t, -event.normal});
		}
    }

	m_collisionEvents.clear();
}
