#include "ThrashCacheComponent.h"
#include <imgui.h>

dae::ThrashCacheComponent::ThrashCacheComponent(GameObject& owner)
	: Component(owner)
{
}

void dae::ThrashCacheComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::ThrashCacheComponent::RenderUI() const
{

	ImGui::ShowDemoWindow(); // For demonstration purposes, do not keep this in your engine
}
