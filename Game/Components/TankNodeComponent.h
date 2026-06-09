#pragma once

#include "Minigin/Components/Component.h"
#include "Minigin/GameObject.h"

enum class TankDirection
{
	None,
	Up,
	Down,
	Left,
	Right
};

class TankNodeComponent final : public dae::Component
{
public:
	TankNodeComponent(dae::GameObject& owner);

	void SetNeighbour(TankDirection direction, dae::GameObject* node);
	dae::GameObject* GetNeighbour(TankDirection direction) const;

	virtual void Start() override {}
	virtual void FixedUpdate(float) override {}
	virtual void Update(float) override {}
	virtual void Render() const override {}

	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	virtual void Load(const dae::ParamMap& params) override;

private:
	dae::GameObject* m_pUpNode{ nullptr };
	dae::GameObject* m_pDownNode{ nullptr };
	dae::GameObject* m_pLeftNode{ nullptr };
	dae::GameObject* m_pRightNode{ nullptr };
};
