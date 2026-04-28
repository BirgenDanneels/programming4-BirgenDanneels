#include "StatsComponent.h"
#include "Minigin/Components/TextComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/ResourceManager.h"

dae::StatsComponent::StatsComponent(GameObject& pOwner)
	: Component(pOwner), m_ptrTextComponent(GetOwner()->GetComponent<TextComponent>())
{
	if (!m_ptrTextComponent)
	{
		auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 15);
		m_ptrTextComponent = GetOwner()->AddComponent<TextComponent>();
		m_ptrTextComponent->Initialize("", font);
	}
}

dae::StatsComponent::~StatsComponent() = default;

void dae::StatsComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::StatsComponent::OnNotify(int stat)
{
	if (m_ptrTextComponent)
		m_ptrTextComponent->SetText(m_preStatString + std::to_string(stat));
}

void dae::StatsComponent::Initialize(std::string preStatString)
{
	m_preStatString = std::move(preStatString);

	m_ptrTextComponent->SetText(m_preStatString + std::to_string(0));
}

std::vector<dae::ParamDefinition> dae::StatsComponent::GetExpectedParams() const
{
	return {
		{ "text", std::string("") }
	};
}

void dae::StatsComponent::Load(const ParamMap& params)
{
	std::string text = GetRequiredParam<std::string>(params, "text");

	Initialize(text);
}
