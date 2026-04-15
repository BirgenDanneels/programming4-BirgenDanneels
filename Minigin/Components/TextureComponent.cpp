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

void dae::TextureComponent::Load(const dae::ParamMap& params)
{
	if (auto it = params.find("texture"); it != params.end())
	{
		if (std::holds_alternative<std::string>(it->second))
		{
			const auto& filename = std::get<std::string>(it->second);
			Initialize(filename);
		}
		else
		{
			throw std::runtime_error("Invalid parameter type for TextureComponent: Texture should be a string");
		}
	}
	else
	{
		throw std::runtime_error("Missing required parameter for TextureComponent: Texture");
	}
}
