#include "StatsComponent.h"
#include "Minigin/Components/TextComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/ResourceManager.h"

StatsComponent::StatsComponent(dae::GameObject& pOwner)
	: dae::Component(pOwner), m_ptrTextComponent(GetOwner()->GetComponent<dae::TextComponent>())
{
	if (!m_ptrTextComponent)
	{
		auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 15);
		m_ptrTextComponent = GetOwner()->AddComponent<dae::TextComponent>();
		m_ptrTextComponent->Initialize("", font);
	}
}

StatsComponent::~StatsComponent() = default;

void StatsComponent::OnNotify(int stat)
{
	if (m_ptrTextComponent)
		m_ptrTextComponent->SetText(m_preStatString + std::to_string(stat));
}

void StatsComponent::Initialize(std::string preStatString)
{
	m_preStatString = std::move(preStatString);

	m_ptrTextComponent->SetText(m_preStatString + std::to_string(0));
}

std::vector<dae::ParamDefinition> StatsComponent::GetExpectedParams() const
{
	return {
		{ "text", std::string("") }
	};
}

void StatsComponent::Load(const dae::ParamMap& params)
{
	std::string text = dae::GetRequiredParam<std::string>(params, "text");

	Initialize(text);
}
