#include "SpriteComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Minigin/Loading/LoadingHelpers.h"

dae::SpriteComponent::SpriteComponent(GameObject& refOwner)
	:Component(refOwner)
{
}

void dae::SpriteComponent::Initialize(const std::string& filename, SDL_FRect srcRect, bool centered, float xOffset, float yOffset)
{
	m_spriteSheet = ResourceManager::GetInstance().LoadTexture(filename);
	m_srcRect = srcRect;

	if (centered)
	{
		m_xOffset = -srcRect.w / 2.0f;
		m_yOffset = -srcRect.h / 2.0f;
	}
	else
	{
		m_xOffset = xOffset;
		m_yOffset = yOffset;
	}
}

std::vector<dae::ParamDefinition> dae::SpriteComponent::GetExpectedParams() const
{

	return {{ "filename", std::string{} },
			{ "srcX", float{} },
			{ "srcY", float{} },
			{ "srcWidth", float{} },
			{ "srcHeight", float{} },
			{ "centered", bool{} },
			{ "xOffset", float{} },
			{ "yOffset", float{} }};
}

void dae::SpriteComponent::Load(const ParamMap& params)
{
	std::string filename = GetRequiredParam<std::string>(params, "filename");
	SDL_FRect srcRect{};
	srcRect.x = GetRequiredParam<float>(params, "srcX");
	srcRect.y = GetRequiredParam<float>(params, "srcY");
	srcRect.w = GetRequiredParam<float>(params, "srcWidth");
	srcRect.h = GetRequiredParam<float>(params, "srcHeight");

	bool centered = GetRequiredParam<bool>(params, "centered");
	float xOffset = GetRequiredParam<float>(params, "xOffset");
	float yOffset = GetRequiredParam<float>(params, "yOffset");

	Initialize(filename, srcRect, centered, xOffset, yOffset);
}

void dae::SpriteComponent::Render() const
{
	if (m_spriteSheet != nullptr)
	{
		const auto& transform = GetOwner()->GetTransform();
		
		const auto& pos = transform.GetWorldPosition();
		const float angle = transform.GetWorldRotation();
		const auto& scale = transform.GetWorldScale();
		
		// Apply Transform scale
		float finalWidth = m_srcRect.w * scale.x;
		float finalHeight = m_srcRect.h * scale.y;

		float scaledOffsetX = m_xOffset * scale.x;
		float scaledOffsetY = m_yOffset * scale.y;

		SDL_FPoint centerPoint = { -scaledOffsetX, -scaledOffsetY };

		SDL_FRect dst{
			pos.x + scaledOffsetX,
			pos.y + scaledOffsetY,
			finalWidth,
			finalHeight
		};

		Renderer::GetInstance().RenderTexture(*m_spriteSheet, dst, angle, &m_srcRect, &centerPoint);
	}
}
