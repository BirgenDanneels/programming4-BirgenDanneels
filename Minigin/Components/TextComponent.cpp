#include "TextComponent.h"
#include "Font.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>
#include"Renderer.h"

dae::TextComponent::TextComponent(dae::GameObject& refOwner)
	:TextureComponent(refOwner, "TextComponent")
{
}

void dae::TextComponent::FixedUpdate(float fixedDeltaTime)
{
	(void)fixedDeltaTime;
}

void dae::TextComponent::Update(float deltaTime)
{
	(void)deltaTime;

	if (m_needsUpdate)
	{
		const auto surf = TTF_RenderText_Blended(m_font->GetFont(), m_text.c_str(), m_text.size(), m_color);
		if (surf == nullptr)
		{
			throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());
		}
		auto texture = SDL_CreateTextureFromSurface(dae::Renderer::GetInstance().GetSDLRenderer(), surf);
		if (texture == nullptr)
		{
			throw std::runtime_error(std::string("Create text texture from surface failed: ") + SDL_GetError());
		}
		SDL_DestroySurface(surf);

		UpdateTexture(std::make_shared<dae::Texture2D>(texture));
	}
}

void dae::TextComponent::Render() const
{
	TextureComponent::Render();
}

void dae::TextComponent::Initialize(const std::string& text, std::shared_ptr<dae::Font> font, float xOffset, float yOffset)
{
	m_text = text;
	m_font = std::move(font);
	SetOffset(xOffset, yOffset);
	
	m_needsUpdate = true;

}

void dae::TextComponent::SetText(const std::string& text)
{
	m_text = text;
	m_needsUpdate = true;
}

void dae::TextComponent::SetColor(const SDL_Color& color)
{
	m_color = color;
	m_needsUpdate = true;
}


