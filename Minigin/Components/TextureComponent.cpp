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

void dae::TextureComponent::FixedUpdate(float fixedDeltaTime)
{
	(void)fixedDeltaTime;
}

void dae::TextureComponent::Update(float deltaTime)
{
	(void)deltaTime;
}

void dae::TextureComponent::Initialize(const std::string& filename, float xOffset, float yOffset)
{
	m_texture = dae::ResourceManager::GetInstance().LoadTexture(filename);
	m_xOffset = xOffset;
	m_yOffset = yOffset;
}

void dae::TextureComponent::Render() const
{
	if (m_texture != nullptr)
	{
		const auto& pos = GetOwner()->GetWorldPosition();
		dae::Renderer::GetInstance().RenderTexture(*m_texture, pos.r, pos.g);
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
		{ "texture", std::string("") }
	};
}

void dae::TextureComponent::Load(const dae::ParamMap& params)
{
	const std::string filename = GetRequiredParam<std::string>(params, "texture");

	Initialize(filename);
}
