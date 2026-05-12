#include "Collider.h"
#include "PhysicsManager.h"
#include "Minigin/GameObject.h"
#include "Minigin/Transform.h"
#include "Minigin/Sound/ServiceLocator.h"
#include <imgui.h>
#include "Minigin/Loading/LoadingHelpers.h"

dae::Collider::Collider(GameObject& refOwner)
	: Component(refOwner)
{
}

dae::Collider::~Collider()
{
	ServiceLocator::GetPhysicsManager().UnregisterCollider(this);
}

void dae::Collider::Start()
{
	ServiceLocator::GetPhysicsManager().RegisterCollider(this);
}

void dae::Collider::RenderUI()
{
	if (!m_debugDraw)
		return;

	auto* drawList = ImGui::GetBackgroundDrawList();
	if (!drawList)
		return;

	const auto worldPos = GetWorldPosition();
	const ImVec2 imguiPos{ worldPos.x, worldPos.y };

	// Draw box collider
	const ImVec2 min{ worldPos.x - m_size.x * 0.5f, worldPos.y - m_size.y * 0.5f };
	const ImVec2 max{ worldPos.x + m_size.x * 0.5f, worldPos.y + m_size.y * 0.5f };

	const ImU32 color = IM_COL32(0, 255, 0, 200); // Green with transparency
	drawList->AddRect(min, max, color, 0.0f);
	
}

void dae::Collider::InitializeBoxCollider(float width, float height, float xOffset, float yOffset) 
{    
	m_size = { width, height };
	m_offset = { xOffset, yOffset };
}

std::vector<dae::ParamDefinition> dae::Collider::GetExpectedParams() const
{
	return { {"width", 0.0f}, {"height", 0.0f}, {"xOffset", 0.0f}, {"yOffset", 0.0f} };
}

void dae::Collider::Load(const ParamMap& params)
{
	float width = GetRequiredParam<float>(params, "width");
	float height = GetRequiredParam<float>(params, "height");
	float xOffset = GetOptionalParam<float>(params, "xOffset", 0.0f);
	float yOffset = GetOptionalParam<float>(params, "yOffset", 0.0f);
	InitializeBoxCollider(width, height, xOffset, yOffset);
}

glm::vec2 dae::Collider::GetWorldPosition() const
{
	auto& transform = GetOwner()->GetTransform();
	const auto& worldPos = transform.GetWorldPosition();
	return { worldPos.x + m_offset.x, worldPos.y + m_offset.y };
}

dae::AABB dae::Collider::GetBounds() const
{
	return AABB{
		.min = GetOffset() - (m_size * 0.5f),
		.max = GetOffset() + (m_size * 0.5f)
	};
}