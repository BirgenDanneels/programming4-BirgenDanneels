#include "TextComponent.h"
#include "Font.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <stdexcept>
#include"Renderer.h"
#include "ResourceManager.h"

dae::TextComponent::TextComponent(dae::GameObject& refOwner)
	:TextureComponent(refOwner)
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
		
		if (m_centered)
			CalculateOffsetsCentered();

		m_needsUpdate = false; 
	}
}

void dae::TextComponent::Render() const
{
	TextureComponent::Render();
}

void dae::TextComponent::Initialize(const std::string& text, std::shared_ptr<dae::Font> font, bool centered, float xOffset, float yOffset)
{
	m_text = text;
	m_font = std::move(font);
	SetOffset(xOffset, yOffset);

	m_centered = centered;
	
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

std::vector<dae::ParamDefinition> dae::TextComponent::GetExpectedParams() const
{
	return {
		{ "text", std::string("") },
		{ "font", std::string("") },
		{ "size", 20 },
		{ "color", std::vector<int>{ 255, 255, 255, 255 } },
		{  "centered", false  }
	};
}

void dae::TextComponent::Load(const ParamMap& params)
{
	std::string text = GetRequiredParam<std::string>(params, "text");
	std::string fontFile = GetRequiredParam<std::string>(params, "font");
	bool centered = GetOptionalParam<bool>(params, "centered", false);

	int size = GetOptionalParam<int>(params, "size", 20);

	std::vector<int> colorVec = GetOptionalParam<std::vector<int>>(params, "color", { 255, 255, 255, 255 });
	SDL_Color color{};
	color.r = static_cast<Uint8>(colorVec[0]);
	color.g = static_cast<Uint8>(colorVec[1]);
	color.b = static_cast<Uint8>(colorVec[2]);
	color.a = static_cast<Uint8>(colorVec[3]);

	Initialize(text, dae::ResourceManager::GetInstance().LoadFont(fontFile, static_cast<uint8_t>(size)), centered);
	SetColor(color);
}


