#pragma once
#include "Minigin/Components/Component.h"
#include "Minigin/Events/Subject.h"
#include <memory>
#include <glm/vec2.hpp>
#include <SDL3/SDL.h>

namespace dae {
	class TextComponent;
	class TextureComponent;
}

class ButtonComponent final : public dae::Component
{
public:

	enum class ButtonDirection
	{
		Up,
		Down,
		Left,
		Right
	};

	ButtonComponent(dae::GameObject& owner);
	~ButtonComponent() override = default;

	void Start() override;
	void Update(float) override {};
	void FixedUpdate(float) override {};
	void Render() const override;

	void Initialize(dae::GameObject* up, dae::GameObject* down, dae::GameObject* left, dae::GameObject* right, std::string text, SDL_Color color, glm::vec2 dimensions, std::string eventName);
	std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	void Load(const dae::ParamMap& params) override;

	void SetButtonText(const std::string& text);
	void SetButtonDimensions(float width, float height);
	void SetButtonColor(const SDL_Color& color);

	ButtonComponent* GetNeighbor(ButtonDirection dir) const;
	void SetNeighbor(ButtonDirection dir, ButtonComponent* neighbor);

	void OnFocus();
	void OnUnfocus();

	void Press();

	dae::Subject<ButtonComponent*> OnPressed;

private:

	ButtonComponent* m_upNeighbor = nullptr;
	ButtonComponent* m_downNeighbor = nullptr;
	ButtonComponent* m_leftNeighbor = nullptr;
	ButtonComponent* m_rightNeighbor = nullptr;

	dae::TextComponent* m_textComponent = nullptr;

	float m_width = 100.0f;
	float m_height = 50.0f;
	float m_ButtonScale = 1.f;
	unsigned int m_EventId = 0;
	SDL_Color m_backgroundColor{ 100, 100, 100, 255 };
};

