#include "TextureComponent.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Renderer.h"

dae::TextureComponent::TextureComponent(dae::GameObject& refOwner)
	:Component(refOwner)
{
}

void dae::TextureComponent::UpdateTexture(std::shared_ptr<dae::Texture2D> texture)
{
	m_texture = texture;
}

void dae::TextureComponent::CalculateOffsetsCentered()
{
	float texWidth = 0.0f;
	float texHeight = 0.0f;
	SDL_GetTextureSize(m_texture->GetSDLTexture(), &texWidth, &texHeight);

	//Rounding offsets to prevent blurriness
	m_xOffset = std::round(-texWidth / 2.0f);
	m_yOffset = std::round(-texHeight / 2.0f);
}

void dae::TextureComponent::FixedUpdate(float fixedDeltaTime)
{
	(void)fixedDeltaTime;
}

void dae::TextureComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::TextureComponent::Initialize(const std::string& filename, bool centered, float xOffset, float yOffset)
{
	m_texture = dae::ResourceManager::GetInstance().LoadTexture(filename);

	if(centered)
	{
		CalculateOffsetsCentered();
	}
	else
	{
		m_xOffset = xOffset;
		m_yOffset = yOffset;
	}
}

void dae::TextureComponent::Render() const
{
	if (m_texture != nullptr)
	{
		const auto& transform = GetOwner()->GetTransform();
		
		const auto& pos = transform.GetWorldPosition();
		const float angle = transform.GetWorldRotation();
		const auto& scale = transform.GetWorldScale();

		// Get base texture size
		float texWidth = 0.0f;
		float texHeight = 0.0f;
		SDL_GetTextureSize(m_texture->GetSDLTexture(), &texWidth, &texHeight);

		// Apply Transform scale
		float finalWidth = texWidth * scale.x;
		float finalHeight = texHeight * scale.y;
		
		float scaledOffsetX = m_xOffset * scale.x;
		float scaledOffsetY = m_yOffset * scale.y;

		// The rotation center is relative to the top-left of the destination rect.
		// Reversing the offset forces the texture to pivot exactly around the GameObject's world position.
		SDL_FPoint centerPoint = { -scaledOffsetX, -scaledOffsetY };

		dae::Renderer::GetInstance().RenderTexture(
			*m_texture, 
			pos.x + scaledOffsetX, 
			pos.y + scaledOffsetY, 
			finalWidth, 
			finalHeight, 
			angle,
			&centerPoint
		);
	}
}

void dae::TextureComponent::SetOffset(float x, float y)
{
	m_xOffset = x;
	m_yOffset = y;
}

std::vector<dae::ParamDefinition> dae::TextureComponent::GetExpectedParams() const
{
	return {
		{ "texture", std::string("") },
		{ "centered", bool(true)}
	};
}

void dae::TextureComponent::Load(const dae::ParamMap& params)
{
	const std::string filename = GetRequiredParam<std::string>(params, "texture");
	const bool centered = GetRequiredParam<bool>(params, "centered");

	Initialize(filename, centered);
}
