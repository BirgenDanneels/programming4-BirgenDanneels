#pragma once

#include "Minigin/Components/Component.h"
#include "TankNodeComponent.h"
#include "Minigin/Events/Subject.h"

namespace dae
{
	class Rigidbody;
	class GameObject;
}

class NodeMovementComponent final : public dae::Component
{
public:
	NodeMovementComponent(dae::GameObject& owner);

	void Start() override;
	void FixedUpdate(float fixedDeltaTime) override;
	void Update(float) override {}
	void Render() const override {}

	void Initialize(float speed, dae::GameObject* startNode, float nodeReachDistance = 1.0f);

	void SetHeldMoveInput(const glm::vec3& input);
	void SetHeldDirection(TankDirection direction);

	void SetSpeed(float speed) { m_Speed = speed; }
	float GetSpeed() const { return m_Speed; }

	void SetCurrentNode(dae::GameObject* node, bool snapToNode = true);
	dae::GameObject* GetCurrentNode() const { return m_pCurrentNode; }
	dae::GameObject* GetTargetNode() const { return m_pTargetNode; }

	TankDirection GetCurrentDirection() const { return m_CurrentDirection; }
	TankDirection GetHeldDirection() const { return m_HeldDirection; }

	bool IsMovingBetweenNodes() const { return m_IsMovingBetweenNodes; }
	bool IsStoppedBetweenNodes() const { return !m_IsMovingBetweenNodes && m_pTargetNode != nullptr; }

	void StopMovement();
	void StopAtCurrentNode();

	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	virtual void Load(const dae::ParamMap& params) override;

	dae::Subject<dae::GameObject*>& OnNodeReached() { return m_NodeReachedSubject; }

private:
	TankDirection InputToDirection(const glm::vec3& input) const;

	bool IsOppositeDirection(TankDirection a, TankDirection b) const;

	void TryChooseNextNode();
	void StartMovingToNode(dae::GameObject* nextNode, TankDirection direction);

	float GetRotationForDirection(TankDirection direction) const;

	void NotifyNodeReached();

	dae::Rigidbody* m_pRigidbody{ nullptr };

	float m_Speed{ 100.0f };
	float m_NodeReachDistance{ 1.0f };

	dae::GameObject* m_pCurrentNode{ nullptr };
	dae::GameObject* m_pTargetNode{ nullptr };

	TankDirection m_CurrentDirection{ TankDirection::None };
	TankDirection m_HeldDirection{ TankDirection::None };

	bool m_IsMovingBetweenNodes{ false };

	dae::Subject<dae::GameObject*> m_NodeReachedSubject;
};
