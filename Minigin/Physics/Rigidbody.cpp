#include "Rigidbody.h"
#include "Collider.h"
#include "PhysicsManager.h"
#include "Minigin/GameObject.h"
#include "Minigin/Sound/ServiceLocator.h"
#include "Minigin/Loading/LoadingHelpers.h"

dae::Rigidbody::Rigidbody(GameObject& refOwner)
	: Component(refOwner)
{
}

dae::Rigidbody::~Rigidbody()
{
}

void dae::Rigidbody::Start()
{
	if (Collider* collider = GetOwner()->GetComponent<Collider>())
	{
		collider->SetRigidbody(this);
	}
}

void dae::Rigidbody::FixedUpdate(float)
{
	if (!m_isKinematic)
	{
		// Apply drag
		if (m_drag > 0.0f)
		{
			m_velocity *= (1.0f - m_drag);
		}

		// Apply gravity
		if (m_useGravity)
		{
			m_acceleration.y += ServiceLocator::GetPhysicsManager().GetGravity() * m_gravityScale;
		}

		// Apply acceleration
		m_velocity += m_acceleration;
		m_acceleration = { 0.0f, 0.0f }; // Reset for next frame
	}

	// Apply constraints
	if (m_freezeX) m_velocity.x = 0.0f;
	if (m_freezeY) m_velocity.y = 0.0f;
}

void dae::Rigidbody::Initialize(bool useGravity, bool isKinematic, bool canBounce)
{
	m_useGravity = useGravity;
	m_isKinematic = isKinematic;
	m_canBounce = canBounce;
}

std::vector<dae::ParamDefinition> dae::Rigidbody::GetExpectedParams() const
{
	return { {"useGravity", false}, 
		{"isKinematic", true}, 
		{"canBounce", false} };
}

void dae::Rigidbody::Load(const ParamMap& params)
{
	m_useGravity = GetOptionalParam<bool>(params, "useGravity", false);
	m_isKinematic = GetOptionalParam<bool>(params, "isKinematic", true);
	m_canBounce = GetOptionalParam<bool>(params, "canBounce", false);
}

void dae::Rigidbody::SetDrag(float drag)
{
	m_drag = std::clamp(drag, 0.0f, 1.0f);
}

void dae::Rigidbody::SetConstraints(bool freezeX, bool freezeY)
{
	m_freezeX = freezeX;
	m_freezeY = freezeY;
}

void dae::Rigidbody::SetVelocity(const glm::vec2& velocity) 
{
	m_velocity = velocity;
}

void dae::Rigidbody::AddForce(const glm::vec2& force)
{
	m_acceleration += force;
}
