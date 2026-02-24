#include "GameObject.h"
#include <string>
#include "ResourceManager.h"
#include "Renderer.h"

#include "Components/ComponentsInclude.h"


dae::GameObject::GameObject()
	:m_transformComponent{ std::make_unique<dae::TransformComponent>(*this)}, m_ptrParent{ nullptr }
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

void dae::GameObject::SetParent(GameObject* ptrParent)
{
	if (!CheckIfParentIsValid(ptrParent))
		return;

	// Remove this GameObject from the current parent (if it has one)
	if(m_ptrParent != nullptr)
		m_ptrParent->RemoveChild(this);

	m_ptrParent = ptrParent;
	if (m_ptrParent != nullptr)
		m_ptrParent->AddChild(this);
}

const std::vector<dae::GameObject*>& dae::GameObject::GetChildren() const
{
	return m_vectChildren;
}

void dae::GameObject::RemoveChild(GameObject* ptrChild)
{
	auto it = std::find(m_vectChildren.begin(), m_vectChildren.end(), ptrChild);
	if (it != m_vectChildren.end())
		m_vectChildren.erase(it);
}

void dae::GameObject::AddChild(GameObject* ptrChild)
{
	m_vectChildren.push_back(ptrChild);
}

bool dae::GameObject::CheckIfParentIsValid(GameObject* ptrParent) const
{
	//Check if the new parent is the same as the current parent or if its the same as this GameObject
	if (ptrParent == m_ptrParent || ptrParent == this)
		return false;

	//Check if the new parent is a (indirect) child of this GameObject
	if (IsDescendant(ptrParent))
		return false;

	return true;
}

bool dae::GameObject::HasChild(GameObject* ptrChild) const
{
	if (ptrChild == nullptr) return false;

	//use find_if to check if the vector already contains the given pointer
	auto childIt = std::find_if(m_vectChildren.begin(), m_vectChildren.end(),
		[ptrChild](GameObject* ptr) {return ptr == ptrChild; });

	//if the iterator equals the the end of the child vector then the parent isnt in the child vector
	if (childIt != m_vectChildren.end())
		return true;

	return false;
}

bool dae::GameObject::IsDescendant(GameObject* ptrChild) const
{
	if (ptrChild == nullptr) return false;
	for (auto child : m_vectChildren)
	{
		if (child == ptrChild) return true;
		if (child->IsDescendant(ptrChild)) return true;
	}
	return false;
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