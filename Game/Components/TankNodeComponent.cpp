#include "TankNodeComponent.h"
#include "Minigin/Loading/LoadingHelpers.h"

TankNodeComponent::TankNodeComponent(dae::GameObject& owner)
	: Component(owner)
{
}

void TankNodeComponent::SetNeighbour(TankDirection direction, dae::GameObject* node)
{
	switch (direction)
	{
	case TankDirection::Up:
		m_pUpNode = node;
		break;

	case TankDirection::Down:
		m_pDownNode = node;
		break;

	case TankDirection::Left:
		m_pLeftNode = node;
		break;

	case TankDirection::Right:
		m_pRightNode = node;
		break;

	case TankDirection::None:
	default:
		break;
	}
}

dae::GameObject* TankNodeComponent::GetNeighbour(TankDirection direction) const
{
	switch (direction)
	{
	case TankDirection::Up:
		return m_pUpNode;

	case TankDirection::Down:
		return m_pDownNode;

	case TankDirection::Left:
		return m_pLeftNode;

	case TankDirection::Right:
		return m_pRightNode;

	case TankDirection::None:
	default:
		return nullptr;
	}
}

std::vector<dae::ParamDefinition> TankNodeComponent::GetExpectedParams() const
{
	dae::GameObject* nullPtr{ nullptr };

	return
	{
		{ "up", nullPtr },
		{ "down", nullPtr },
		{ "left", nullPtr },
		{ "right", nullPtr }
	};
}

void TankNodeComponent::Load(const dae::ParamMap& params)
{
	m_pUpNode = GetOptionalParam<dae::GameObject*>(params, "up", nullptr);
	m_pDownNode = GetOptionalParam<dae::GameObject*>(params, "down", nullptr);
	m_pLeftNode = GetOptionalParam<dae::GameObject*>(params, "left", nullptr);
	m_pRightNode = GetOptionalParam<dae::GameObject*>(params, "right", nullptr);
}
