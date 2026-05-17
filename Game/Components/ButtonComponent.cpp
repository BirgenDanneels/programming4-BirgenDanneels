#include "ButtonComponent.h"
#include "Minigin/GameObject.h"
#include "Minigin/Scene.h"
#include "Minigin/Loading/LoadingHelpers.h"
#include "Minigin/Components/TextComponent.h"
#include "Minigin/Components/TextureComponent.h"
#include "Minigin/Renderer.h"
#include "Minigin/ResourceManager.h"
#include "Minigin/Font.h"
#include "Minigin/Events/GameEventQueue.h"

ButtonComponent::ButtonComponent(dae::GameObject& owner)
	: Component(owner)
{
}

void ButtonComponent::Start()
{
}

void ButtonComponent::Render() const
{
	auto pos = GetOwner()->GetTransform().GetWorldPosition();

	float scaledWidth = m_width * m_ButtonScale;
	float scaledHeight = m_height * m_ButtonScale;

	// Center the rect on the position
	pos.x -= scaledWidth / 2;
	pos.y -= scaledHeight / 2;

	SDL_FRect rect{ pos.x, pos.y, scaledWidth, scaledHeight };

	auto renderer = dae::Renderer::GetInstance().GetSDLRenderer();
	SDL_SetRenderDrawColor(renderer, m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
	SDL_RenderFillRect(renderer, &rect);
}

void ButtonComponent::SetButtonText(const std::string& text)
{
	m_textComponent->SetText(text);
}

void ButtonComponent::Initialize(dae::GameObject* up, dae::GameObject* down, dae::GameObject* left, dae::GameObject* right, std::string text, SDL_Color color, glm::vec2 dimensions, std::string eventName)
{
	m_upNeighbor = up ? up->GetComponent<ButtonComponent>() : nullptr;
	m_downNeighbor = down ? down->GetComponent<ButtonComponent>() : nullptr;
	m_leftNeighbor = left ? left->GetComponent<ButtonComponent>() : nullptr;
	m_rightNeighbor = right ? right->GetComponent<ButtonComponent>() : nullptr;

	if(!eventName.empty())
		m_EventId = dae::Event::ToEventID(eventName);


	m_textComponent = GetOwner()->GetComponent<dae::TextComponent>();
	if (m_textComponent == nullptr)
	{
		m_textComponent = GetOwner()->AddComponent<dae::TextComponent>();
		m_textComponent->Initialize(text, dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20), true);
	}

	SetButtonText(text);
	SetButtonColor(color);
	SetButtonDimensions(dimensions.x, dimensions.y);
}

std::vector<dae::ParamDefinition> ButtonComponent::GetExpectedParams() const
{
	return { { "up", (dae::GameObject*) nullptr },
			 { "down", (dae::GameObject*) nullptr },
			 { "left", (dae::GameObject*) nullptr },
			 { "right", (dae::GameObject*) nullptr },
			 { "text", std::string{} },
			 { "color", std::vector<int>{} },
			 { "width", float{} },
			 { "height", float{} },
			 { "eventName", std::string{} } };
}

void ButtonComponent::Load(const dae::ParamMap& params)
{
	SDL_Color color{ 100, 100, 100, 255 }; // Default color
	auto colorVec = dae::GetRequiredParam<std::vector<int>>(params, "color");

	if (colorVec.size() >= 3)
	{
		color.r = static_cast<Uint8>(colorVec[0]);
		color.g = static_cast<Uint8>(colorVec[1]);
		color.b = static_cast<Uint8>(colorVec[2]);
		if (colorVec.size() >= 4)
			color.a = static_cast<Uint8>(colorVec[3]);
	}

	glm::vec2 dimensions{ dae::GetRequiredParam<float>(params, "width"),
		dae::GetRequiredParam<float>(params, "height") }; // Default dimensions

	Initialize(dae::GetOptionalParam<dae::GameObject*>(params, "up", nullptr),
		dae::GetOptionalParam<dae::GameObject*>(params, "down", nullptr),
		dae::GetOptionalParam<dae::GameObject*>(params, "left", nullptr),
		dae::GetOptionalParam<dae::GameObject*>(params, "right", nullptr),
		dae::GetRequiredParam<std::string>(params, "text"),
		color,
		dimensions,
		dae::GetOptionalParam<std::string>(params, "eventName", "")
		);
}

ButtonComponent* ButtonComponent::GetNeighbor(ButtonDirection dir) const
{
	switch (dir)
	{
	case ButtonComponent::ButtonDirection::Up:
		return m_upNeighbor;
	case ButtonComponent::ButtonDirection::Down:
		return m_downNeighbor;
	case ButtonComponent::ButtonDirection::Left:
		return m_leftNeighbor;
	case ButtonComponent::ButtonDirection::Right:
		return m_rightNeighbor;
	default:
		return nullptr;
	}
}

void ButtonComponent::SetNeighbor(ButtonDirection dir, ButtonComponent* neighbor)
{
	switch (dir)
	{
	case ButtonComponent::ButtonDirection::Up:
		m_upNeighbor = neighbor;
		break;
	case ButtonComponent::ButtonDirection::Down:
		m_downNeighbor = neighbor;
		break;
	case ButtonComponent::ButtonDirection::Left:
		m_leftNeighbor = neighbor;
		break;
	case ButtonComponent::ButtonDirection::Right:
		m_rightNeighbor = neighbor;
		break;
	default:
		break;
	}
}

void ButtonComponent::OnFocus()
{
	// Temporary visual feedback for focus
	// TODO: Replace with proper visual feedback
	m_ButtonScale = 1.5f;
}

void ButtonComponent::OnUnfocus()
{
	// Temporary visual feedback for unfocus
	// TODO: Replace with proper visual feedback
	m_ButtonScale = 1.f;
}

void ButtonComponent::Press()
{
	if(m_EventId != 0)
		GetOwner()->GetScene()->GetGameEventQueue().Enqueue(dae::Event{ m_EventId, {this} });

	OnPressed.NotifyObservers(this);
}

void ButtonComponent::SetButtonDimensions(float width, float height)
{
	m_width = width;
	m_height = height;
}

void ButtonComponent::SetButtonColor(const SDL_Color& color)
{
	m_backgroundColor = color;
}
