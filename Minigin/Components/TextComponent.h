#pragma once
#include "TextureComponent.h"
#include <SDL3/SDL.h>
#include "Loading/Interfaces/IComponentLoadable.h"

namespace dae
{
	class Font;

	class TextComponent : public TextureComponent
	{
	public:
		TextComponent() = delete;
		TextComponent(dae::GameObject& refOwner);

		TextComponent(const TextComponent& other) = delete;
		TextComponent(TextComponent&& other) = delete;
		TextComponent& operator=(const TextComponent& other) = delete;
		TextComponent& operator=(TextComponent&& other) = delete;

		~TextComponent() = default;

		virtual void FixedUpdate(float fixedDeltaTime) override;
		virtual void Update(float deltaTime) override;

		virtual void Render() const override;

		using TextureComponent::Initialize;
		virtual void Initialize(const std::string& text, std::shared_ptr<dae::Font> font, float xOffset = 0, float yOffset = 0);

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

		// Inherited via IComponentLoadable
		virtual std::vector<ParamDefinition> GetExpectedParams() const override;
		virtual void Load(const ParamMap& params) override;

	private:

		std::string m_text;
		SDL_Color m_color{ 255, 255, 255, 255 };
		std::shared_ptr<dae::Font> m_font;

		bool m_needsUpdate{ true };
	};
}

