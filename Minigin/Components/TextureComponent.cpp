#include "TextureComponent.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Renderer.h"

dae::TextureComponent::TextureComponent(dae::GameObject& refOwner)
	:Component(refOwner, "TextureComponent")
{
}

dae::TextureComponent::TextureComponent(dae::GameObject& refOwner, const std::string& componentName)
	:Component(refOwner, componentName)
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
		//TODO: change to world pos instead of local position
		const auto& pos = GetOwner()->GetLocalPosition();
		dae::Renderer::GetInstance().RenderTexture(*m_texture, pos.r, pos.g);
	}
}

void dae::TextureComponent::SetOffset(float x, float y)
{
	m_xOffset = x;
	m_yOffset = y;
}
