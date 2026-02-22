#include "GameObject.h"
#include <string>
#include "ResourceManager.h"
#include "Renderer.h"

#include "Components/ComponentsInclude.h"


dae::GameObject::GameObject()
	:m_transformComponent{ std::make_unique<dae::TransformComponent>(*this)}
{
}

dae::GameObject::~GameObject() = default;

const glm::vec3& dae::GameObject::GetLocalPosition()
{
	return m_transformComponent->GetPosition();
}

void dae::GameObject::SetLocalPosition(const glm::vec3& localPos)
{
	m_transformComponent->SetPosition(localPos);
}

void dae::GameObject::SetLocalPosition(int x, int y)
{
	SetLocalPosition(glm::vec3(x, y, 0));
}

void dae::GameObject::Update(float deltaTime)
{
	for (const auto& comp : m_components)
	{
		comp->Update(deltaTime);
	}
}

void dae::GameObject::FixedUpdate(float fixedDeltaTime)
{
	for (const auto& comp : m_components)
	{
		comp->FixedUpdate(fixedDeltaTime);
	}
}

void dae::GameObject::Render() const
{
	for (const auto& comp : m_components)
	{
		if (comp)
		{
			comp->Render();
		}
	}
}