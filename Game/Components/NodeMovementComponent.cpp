#include "NodeMovementComponent.h"

#include "Minigin/GameObject.h"
#include "Minigin/Physics/Rigidbody.h"
#include "Minigin/Loading/LoadingHelpers.h"

#include <glm/glm.hpp>
#include <cmath>

NodeMovementComponent::NodeMovementComponent(dae::GameObject& owner)
	: Component(owner)
{
	//TODO: remove this
	if (!GetOwner()->GetComponent<dae::Rigidbody>())
	{
		GetOwner()->AddComponent<dae::Rigidbody>()->Initialize(false, true);
	}
}

void NodeMovementComponent::Start()
{
	m_pRigidbody = GetOwner()->GetComponent<dae::Rigidbody>();

	if (!m_pRigidbody)
	{
		m_pRigidbody = GetOwner()->AddComponent<dae::Rigidbody>();
		m_pRigidbody->Initialize(false, true);
	}

	if (m_pCurrentNode)
	{
		GetOwner()->GetTransform().SetWorldPosition(m_pCurrentNode->GetTransform().GetLocalPosition());
	}
}

void NodeMovementComponent::FixedUpdate(float fixedDeltaTime)
{
	if (!m_pRigidbody)
		return;

	if (!m_IsMovingBetweenNodes || !m_pTargetNode)
		return;

	const glm::vec3 targetPosition = m_pTargetNode->GetTransform().GetWorldPosition();

	m_pRigidbody->MoveTowards(targetPosition, m_Speed, fixedDeltaTime);

	const glm::vec3 currentPosition = GetOwner()->GetTransform().GetWorldPosition();

	if (glm::distance(currentPosition, targetPosition) <= m_NodeReachDistance)
	{
		GetOwner()->GetTransform().SetWorldPosition(targetPosition);

		m_pCurrentNode = m_pTargetNode;
		m_pTargetNode = nullptr;
		m_IsMovingBetweenNodes = false;

		NotifyNodeReached();

		TryChooseNextNode();
	}
}

void NodeMovementComponent::Initialize(float speed, dae::GameObject* startNode, float nodeReachDistance)
{
	m_Speed = speed;
	m_NodeReachDistance = nodeReachDistance;
	m_pCurrentNode = startNode;

	if (m_pCurrentNode)
	{
		GetOwner()->GetTransform().SetWorldPosition(m_pCurrentNode->GetTransform().GetWorldPosition());
	}
}

void NodeMovementComponent::SetHeldMoveInput(const glm::vec3& input)
{
	SetHeldDirection(InputToDirection(input));
}

void NodeMovementComponent::SetHeldDirection(TankDirection newHeldDirection)
{
	if (newHeldDirection == TankDirection::None)
	{
		m_HeldDirection = TankDirection::None;
		StopMovement();
		return;
	}

	m_HeldDirection = newHeldDirection;

	// If moving and the user presses opposite direction, reverse along the same edge
	if (m_IsMovingBetweenNodes &&
		m_CurrentDirection != TankDirection::None &&
		IsOppositeDirection(newHeldDirection, m_CurrentDirection))
	{
		dae::GameObject* oldCurrentNode = m_pCurrentNode;
		m_pCurrentNode = m_pTargetNode;
		m_pTargetNode = oldCurrentNode;

		m_CurrentDirection = newHeldDirection;
		GetOwner()->GetTransform().SetLocalRotation(GetRotationForDirection(newHeldDirection));
		return;
	}

	// If stopped between nodes, only resume forward or reverse.
	if (!m_IsMovingBetweenNodes && m_pTargetNode)
	{
		if (newHeldDirection == m_CurrentDirection)
		{
			m_IsMovingBetweenNodes = true;
		}
		else if (IsOppositeDirection(newHeldDirection, m_CurrentDirection))
		{
			dae::GameObject* oldCurrentNode = m_pCurrentNode;
			m_pCurrentNode = m_pTargetNode;
			m_pTargetNode = oldCurrentNode;

			m_CurrentDirection = newHeldDirection;
			m_IsMovingBetweenNodes = true;

			GetOwner()->GetTransform().SetLocalRotation(GetRotationForDirection(newHeldDirection));
		}

		return;
	}

	// If standing exactly on a node, choose a next node normally
	if (!m_IsMovingBetweenNodes && m_pCurrentNode && !m_pTargetNode)
	{
		TryChooseNextNode();
	}
}

void NodeMovementComponent::SetCurrentNode(dae::GameObject* node, bool snapToNode)
{
	m_pCurrentNode = node;
	m_pTargetNode = nullptr;
	m_CurrentDirection = TankDirection::None;
	m_HeldDirection = TankDirection::None;
	m_IsMovingBetweenNodes = false;

	if (m_pRigidbody)
	{
		m_pRigidbody->SetVelocity(glm::vec2{ 0.f, 0.f });
	}

	if (snapToNode && m_pCurrentNode)
	{
		GetOwner()->GetTransform().SetWorldPosition(m_pCurrentNode->GetTransform().GetWorldPosition());
	}
}

void NodeMovementComponent::StopMovement()
{
	if (m_pRigidbody)
	{
		m_pRigidbody->SetVelocity(glm::vec2{ 0.f, 0.f });
	}

	m_IsMovingBetweenNodes = false;
}

void NodeMovementComponent::StopAtCurrentNode()
{
	m_pTargetNode = nullptr;
	m_IsMovingBetweenNodes = false;
	m_CurrentDirection = TankDirection::None;

	if (m_pRigidbody)
	{
		m_pRigidbody->SetVelocity(glm::vec2{ 0.f, 0.f });
	}
}

TankDirection NodeMovementComponent::InputToDirection(const glm::vec3& input) const
{
	constexpr float deadZone = 0.2f;

	const float absX = std::abs(input.x);
	const float absY = std::abs(input.y);

	if (absX < deadZone&& absY < deadZone)
		return TankDirection::None;

	if (absX > absY)
	{
		return input.x > 0.f
			? TankDirection::Right
			: TankDirection::Left;
	}

	return input.y > 0.f
		? TankDirection::Down
		: TankDirection::Up;
}

bool NodeMovementComponent::IsOppositeDirection(TankDirection a, TankDirection b) const
{
	return
		(a == TankDirection::Up && b == TankDirection::Down) ||
		(a == TankDirection::Down && b == TankDirection::Up) ||
		(a == TankDirection::Left && b == TankDirection::Right) ||
		(a == TankDirection::Right && b == TankDirection::Left);
}

void NodeMovementComponent::TryChooseNextNode()
{
	if (!m_pCurrentNode)
		return;

	TankNodeComponent* nodeComponent = m_pCurrentNode->GetComponent<TankNodeComponent>();
	if (!nodeComponent)
		return;

	if (m_HeldDirection == TankDirection::None)
		return;

	// First try the held direction
	if (dae::GameObject* nextNode = nodeComponent->GetNeighbour(m_HeldDirection))
	{
		StartMovingToNode(nextNode, m_HeldDirection);
		return;
	}

	// If held direction is blocked, continue forward
	if (m_CurrentDirection != TankDirection::None)
	{
		if (dae::GameObject* forwardNode = nodeComponent->GetNeighbour(m_CurrentDirection))
		{
			StartMovingToNode(forwardNode, m_CurrentDirection);
			return;
		}
	}

	// Held direction blocked and no forward node
	StopAtCurrentNode();
}

void NodeMovementComponent::StartMovingToNode(dae::GameObject* nextNode, TankDirection direction)
{
	if (!nextNode)
		return;

	m_pTargetNode = nextNode;
	m_CurrentDirection = direction;
	m_IsMovingBetweenNodes = true;

	GetOwner()->GetTransform().SetLocalRotation(GetRotationForDirection(direction));
}

float NodeMovementComponent::GetRotationForDirection(TankDirection direction) const
{
	switch (direction)
	{
	case TankDirection::Right:
		return 0.f;

	case TankDirection::Down:
		return 90.f;

	case TankDirection::Left:
		return 180.f;

	case TankDirection::Up:
		return 270.f;

	case TankDirection::None:
	default:
		return 0.f;
	}
}

void NodeMovementComponent::NotifyNodeReached()
{
	m_NodeReachedSubject.NotifyObservers(m_pCurrentNode);
}

std::vector<dae::ParamDefinition> NodeMovementComponent::GetExpectedParams() const
{
	dae::GameObject* nullPtr{ nullptr };

	return
	{
		{ "speed", 100.0f },
		{ "startNode", nullPtr },
		{ "nodeReachDistance", 1.0f }
	};
}

void NodeMovementComponent::Load(const dae::ParamMap& params)
{
	const float speed = GetRequiredParam<float>(params, "speed");
	dae::GameObject* startNode = GetRequiredParam<dae::GameObject*>(params, "startNode");
	const float nodeReachDistance = GetRequiredParam<float>(params, "nodeReachDistance");

	Initialize(speed, startNode, nodeReachDistance);
}
